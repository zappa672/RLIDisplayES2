#include "radarengine.h"

#include "../../common/properties.h"

#include <QFile>
#include <QDateTime>

#include <QOpenGLExtraFunctions>

using namespace RLI;

RadarEngine::RadarEngine(const State& state, Layout* layout, QOpenGLContext* context, QObject* parent)
  : FboLayerBase(layout->circle.box_rect.size(), context, parent) {

  _palette = new RadarPalette(context, this);

  _program = new QOpenGLShaderProgram(this);

  glGenVertexArrays(1, &_vao_id);
  glGenBuffers(ATTR_COUNT, _vbo_ids);
  glGenBuffers(1, &_eab_id);

  initShader();  

  resizeData(state);
}

RadarEngine::~RadarEngine() {
  delete _program;

  glDeleteVertexArrays(1, &_vao_id);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids);
  glDeleteBuffers(1, &_eab_id);

  delete _palette;
}

void RadarEngine::clearTexture() {
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);

  FboLayerBase::clear(1.f, 1.f, 1.f, 0.f, 0.f);
}

void RadarEngine::onBrightnessChanged(int br) {
  _palette->setBrightness(br);
}

void RadarEngine::initShader() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/radar.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/radar.frag.glsl");

  _program->link();
  _program->bind();

  _unif_locs[UNIF_MVP_MATRIX]     = _program->uniformLocation("mvp_matrix");
  _unif_locs[UNIF_TEXTURE]        = _program->uniformLocation("texture");
  _unif_locs[UNIF_THREASHOLD]     = _program->uniformLocation("threashold");
  _unif_locs[UNIF_PELENG_LENGTH]  = _program->uniformLocation("peleng_length");
  _unif_locs[UNIF_PELENG_COUNT]   = _program->uniformLocation("peleng_count");
  _unif_locs[UNIF_FBO_RADIUS]     = _program->uniformLocation("fbo_radius");
  _unif_locs[UNIF_NORTH_SHIFT]    = _program->uniformLocation("north_shift");

  _attr_locs[ATTR_POSITION]       = static_cast<GLuint>(_program->attributeLocation("position"));
  _attr_locs[ATTR_AMPLITUDE]      = static_cast<GLuint>(_program->attributeLocation("amplitude"));

  _program->release();
}

void RadarEngine::resizeData(const State& state) {
  if ( _peleng_count == state.peleng_count
    && _peleng_len == state.peleng_length )
    return;

  _peleng_count = state.peleng_count;
  _peleng_len   = state.peleng_length;

  initBuffers();

  glBindVertexArray(_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glVertexAttribPointer(_attr_locs[ATTR_POSITION], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glVertexAttribPointer( _attr_locs[ATTR_AMPLITUDE], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_AMPLITUDE]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eab_id);

  glBindVertexArray(0);
}

void RadarEngine::initBuffers() {
  std::vector<GLuint> indices;
  std::vector<GLfloat> positions;

  int total = _peleng_count*_peleng_len;

  for (int index = 0; index < _peleng_count; index++) {
    for (int radius = 0; radius < _peleng_len; radius++) {
      int curr_index = index*_peleng_len + radius;
      int prev_index = ((index-1)*_peleng_len + radius + total) % total;

      positions.push_back(curr_index);
      indices.push_back(static_cast<GLuint>(curr_index));
      indices.push_back(static_cast<GLuint>(prev_index));
    }

    GLuint last = indices[indices.size()-1];
    indices.push_back(last);
    indices.push_back((last+1) % static_cast<GLuint>(total));
  }

  auto atr_buf_size = _peleng_count*_peleng_len*static_cast<qopengl_GLintptr>(sizeof(GLfloat));
  auto ind_buf_size = _peleng_count*(2*_peleng_len+2)*static_cast<qopengl_GLintptr>(sizeof(GLuint));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, atr_buf_size, positions.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eab_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_buf_size, indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  clearData();
}

void RadarEngine::resizeTexture(Layout* layout) {
  FboLayerBase::resize(layout->circle.box_rect.size());
  recalcMVP();
}

void RadarEngine::clearData() {
  auto atr_buf_size = _peleng_count*_peleng_len*static_cast<qopengl_GLintptr>(sizeof(GLfloat));
  std::vector<GLfloat> attrs(static_cast<size_t>(_peleng_count*_peleng_len), 0.f);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glBufferData(GL_ARRAY_BUFFER, atr_buf_size, attrs.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _first_recieved_peleng = 0;
  _recieved_peleng_count = 0;

  clearTexture();
}

void RadarEngine::updateData(int offset, int count, GLfloat* amps) {
  qopengl_GLintptr buf_start = offset*_peleng_len*static_cast<qopengl_GLintptr>(sizeof(GLfloat));
  qopengl_GLintptr update_size = count*_peleng_len*static_cast<qopengl_GLintptr>(sizeof(GLfloat));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glBufferSubData(GL_ARRAY_BUFFER, buf_start, update_size, amps);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // New last added peleng  
  if (_recieved_peleng_count == 0)
    _first_recieved_peleng = offset;
  _recieved_peleng_count += count;
}

void RadarEngine::recalcMVP() {
  QSize sz = FboLayerBase::size();

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, sz.width(), 0.f, sz.height(), -255.f, 255.f);

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate( sz.width() / 2.f + static_cast<float>(_center_shift.x())
                     , sz.height() / 2.f + static_cast<float>(_center_shift.y())
                     , 0.f);

  _mvp_matrix = projection*transform;
}

void RadarEngine::updateTexture(const State& state) {
  if (QVector2D(_center_shift - state.center_shift).length() > 0.5f) {
    clearTexture();
    _center_shift = state.center_shift;
    recalcMVP();
  }

  // Calculate which pelengs we should draw
  // --------------------------------------
  if (_recieved_peleng_count == 0)
    return;

  int first_peleng_to_draw = _first_recieved_peleng;
  int last_peleng_to_draw = (_first_recieved_peleng + _recieved_peleng_count - 1) % _peleng_count;

  if (_recieved_peleng_count >= _peleng_count) {
    first_peleng_to_draw = 0;
    last_peleng_to_draw = _peleng_count - 1;
  }
  // --------------------------------------

  // --------------------------------------
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);

  QSize sz = FboLayerBase::size();
  glViewport(0.f, 0.f, sz.width(), sz.height());

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());
  glUseProgram(_program->programId());
  glBindVertexArray(_vao_id);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _palette->texture());

  glUniform1i(_unif_locs[UNIF_TEXTURE], 0);
  glUniformMatrix4fv(_unif_locs[UNIF_MVP_MATRIX], 1, GL_TRUE, _mvp_matrix.data());
  glUniform1f(_unif_locs[UNIF_THREASHOLD], 1.f);

  glUniform1f(_unif_locs[UNIF_PELENG_LENGTH], _peleng_len);
  glUniform1f(_unif_locs[UNIF_PELENG_COUNT], _peleng_count);
  glUniform1f(_unif_locs[UNIF_FBO_RADIUS], sz.width() / 2.f);
  glUniform1f(_unif_locs[UNIF_NORTH_SHIFT], static_cast<float>(state.north_shift));

  if (first_peleng_to_draw <= last_peleng_to_draw) {
    drawPelengs(first_peleng_to_draw, last_peleng_to_draw);
  } else {
    drawPelengs(first_peleng_to_draw, _peleng_count - 1);
    drawPelengs(0, last_peleng_to_draw);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // --------------------------------------  

  _first_recieved_peleng = 0;
  _recieved_peleng_count = 0;
}

void RadarEngine::drawPelengs(int first, int last) {
  // Clear depth when the new cycle begins to avoid the previous circle data
  if (first == 0) {
    glClearDepthf(0.f);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  int inds_to_draw = (last-first+1)*(2*_peleng_len+2);
  size_t inds_offset = static_cast<size_t>(first*(2*_peleng_len+2));

  glDrawElements(GL_TRIANGLE_STRIP, inds_to_draw, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(inds_offset * sizeof(GLuint)));
}

