#include "radarengine.h"

#include "../../common/properties.h"

#include <QFile>
#include <QMatrix4x4>
#include <QDateTime>

using namespace RLI;

RadarEngine::RadarEngine(const State& state, Layout* layout, QOpenGLContext* context, QObject* parent)
  : FboLayerBase(layout->circle.box_rect.size(), context, parent) {

  _palette = new RadarPalette(context, this);

  _program = new QOpenGLShaderProgram(this);

  glGenBuffers(ATTR_COUNT, _vbo_ids);
  glGenBuffers(1, &_ind_vbo_id);  

  initShader();  

  resizeData(state);
}

RadarEngine::~RadarEngine() {
  delete _program;

  glDeleteBuffers(ATTR_COUNT, _vbo_ids);
  glDeleteBuffers(1, &_ind_vbo_id);

  delete _palette;
}

void RadarEngine::clearTexture() {
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());

  glClearDepthf(0.f);
  glClearColor(1.f, 1.f, 1.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

  _attr_locs[ATTR_POSITION]       = _program->attributeLocation("position");
  _attr_locs[ATTR_AMPLITUDE]      = _program->attributeLocation("amplitude");

  _program->release();
}

void RadarEngine::resizeData(const State& state) {
  if ( _peleng_count == state.peleng_count
    && _peleng_len == state.peleng_length )
    return;

  _peleng_count = state.peleng_count;
  _peleng_len = state.peleng_length;

  _has_data = false;

  initBuffers();
}

void RadarEngine::initBuffers() {
  std::vector<GLuint> indices;
  std::vector<GLfloat> positions;

  GLuint total = _peleng_count*_peleng_len;

  for (int index = 0; index < _peleng_count; index++) {
    for (int radius = 0; radius < _peleng_len; radius++) {
      GLuint curr_index = index*_peleng_len + radius;
      GLuint prev_index = ((index-1)*_peleng_len + radius + total) % total;

      positions.push_back(curr_index);
      indices.push_back(curr_index);
      indices.push_back(prev_index);
    }

    GLuint last = indices[indices.size()-1];
    indices.push_back(last);
    indices.push_back((last+1)%total);
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, _peleng_count*_peleng_len*sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*_peleng_count*(_peleng_len+1)*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  clearData();
}

void RadarEngine::resizeTexture(Layout* layout) {
  FboLayerBase::resize(layout->circle.box_rect.size());
}

void RadarEngine::clearData() {
  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glBufferData(GL_ARRAY_BUFFER, _peleng_count*_peleng_len*sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  _draw_circle       = false;
  _has_data          = false;
  _last_drawn_peleng = _peleng_count - 1;
  _last_added_peleng = _peleng_count - 1;
}

void RadarEngine::updateData(int offset, int count, GLfloat* amps) {
  qDebug() << "recieve " << offset << count;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glBufferSubData(GL_ARRAY_BUFFER, offset*_peleng_len*sizeof(GLfloat), count*_peleng_len*sizeof(GLfloat), amps);

  // New last added peleng
  int nlap = (offset + count - 1) % _peleng_count;

  // If we recieved full circle after last draw
  _draw_circle = (_last_added_peleng < _last_drawn_peleng && nlap >= _last_drawn_peleng) || count == _peleng_len;
  _last_added_peleng = nlap;

  if (!_has_data) {
    _draw_circle = false;
    _last_added_peleng = offset % _peleng_count;
    _has_data = true;
  }
}

void RadarEngine::updateTexture(const State& state) {
  if (!_has_data) {
    clearTexture();
    return;
  }

  if (QVector2D(_center_shift - state.center_shift).length() > 0.5f) {
    clearTexture();
    _center_shift = state.center_shift;
  }

  // Calculate which pelengs we should draw
  // --------------------------------------
  if (_last_added_peleng == _last_drawn_peleng && !_draw_circle)
    return;

  int first_peleng_to_draw = (_last_drawn_peleng + 1) % _peleng_count;
  int last_peleng_to_draw = _last_added_peleng % _peleng_count;

  if (_draw_circle)
    first_peleng_to_draw = (_last_added_peleng + 1) % _peleng_count;
  // --------------------------------------

  qDebug() << "draw " << first_peleng_to_draw << last_peleng_to_draw;

  // --------------------------------------

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  QSize sz = FboLayerBase::size();
  glViewport(0.f, 0.f, sz.width(), sz.height());

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, sz.width(), 0.f, sz.height(), -255.f, 255.f);

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate( sz.width() / 2.f + static_cast<float>(_center_shift.x())
                     , sz.height() / 2.f + static_cast<float>(_center_shift.y())
                     , 0.f);

  _program->bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _palette->texture());

  glUniform1i(_unif_locs[UNIF_TEXTURE], 0);
  _program->setUniformValue(_unif_locs[UNIF_MVP_MATRIX], projection*transform);
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
  _program->release();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // --------------------------------------

  _last_drawn_peleng = last_peleng_to_draw;
  _draw_circle = false;
}

void RadarEngine::drawPelengs(int first, int last) {
  // Clear depth when the new cycle begins to avoid the previous circle data
  if (first == 0) {
    glClearDepthf(0.f);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glVertexAttribPointer(_attr_locs[ATTR_POSITION], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glVertexAttribPointer( _attr_locs[ATTR_AMPLITUDE], 1, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(_attr_locs[ATTR_AMPLITUDE]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id);

  glDepthFunc(GL_GREATER);
  glDrawElements( GL_TRIANGLE_STRIP
                , (last-first+1)*(2*_peleng_len+2)
                , GL_UNSIGNED_INT
                , (const GLvoid*)(first*(2*_peleng_len+2) * sizeof(GLuint)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

