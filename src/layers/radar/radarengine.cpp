#include "radarengine.h"

#include "../../common/properties.h"

using namespace RLI;

RadarEngine::RadarEngine(const State& state, const Layout& layout, QOpenGLContext* context, QObject* parent)
  : TextureLayerBase(layout.circle.box_rect, context, parent) {

  _palette = new RadarPalette(context, this);

  glGenVertexArrays(1, &_vao_id);
  glGenBuffers(ATTR_COUNT, _vbo_ids);
  glGenBuffers(1, &_eab_id);

  static const QMap<QString, int> attr_inices
  { { "amplitude",  ATTR_AMPLITUDE }
  , { "position",   ATTR_POSITION  } };

  static const QMap<QString, int> unif_inices
  { { "mvp_matrix",   UNIF_MVP_MATRIX   }
  , { "texture",      UNIF_TEXTURE      }
  , { "threashold",   UNIF_THREASHOLD   }
  , { "peleng_size",  UNIF_PELENG_SIZE  }
  , { "peleng_count", UNIF_PELENG_COUNT } };

  initShaderProgram(":/shaders/radar", attr_inices, unif_inices);
  resizeData(state.peleng_count, state.peleng_size);
}

RadarEngine::~RadarEngine() {
  glDeleteVertexArrays(1, &_vao_id);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids);
  glDeleteBuffers(1, &_eab_id);

  delete _palette;
}

void RadarEngine::clearTexture() {
  TextureLayerBase::clear(0.f, 0.f, 0.f, 0.f, 0.f);
}

void RadarEngine::onBrightnessChanged(int br) {
  _palette->setBrightness(br);
}

void RadarEngine::resizeData(int peleng_count, int peleng_size) {
  if ( _peleng_count == peleng_count && _peleng_size == peleng_size )
    return;

  _peleng_count = peleng_count;
  _peleng_size  = peleng_size;

  initBuffers();

  glBindVertexArray(_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glVertexAttribPointer(attrLoc(ATTR_POSITION), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(attrLoc(ATTR_POSITION));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glVertexAttribPointer(attrLoc(ATTR_AMPLITUDE), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(attrLoc(ATTR_AMPLITUDE));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eab_id);

  glBindVertexArray(0);
}

void RadarEngine::initBuffers() {
  std::vector<GLuint> indices;
  std::vector<GLfloat> positions;

  int total = _peleng_count*_peleng_size;

  for (int index = 0; index < _peleng_count; index++) {
    for (int radius = 0; radius < _peleng_size; radius++) {
      int curr_index = index*_peleng_size + radius;
      int prev_index = ((index-1)*_peleng_size + radius + total) % total;

      positions.push_back(curr_index);
      indices.push_back(static_cast<GLuint>(curr_index));
      indices.push_back(static_cast<GLuint>(prev_index));
    }

    GLuint last = indices[indices.size()-1];
    indices.push_back(last);
    indices.push_back((last+1) % static_cast<GLuint>(total));
  }

  auto atr_buf_size = _peleng_count*_peleng_size*static_cast<qopengl_GLintptr>(sizeof(GLfloat));
  auto ind_buf_size = _peleng_count*(2*_peleng_size+2)*static_cast<qopengl_GLintptr>(sizeof(GLuint));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, atr_buf_size, positions.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eab_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_buf_size, indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  clearData();
}

void RadarEngine::resizeTexture(const Layout& layout) {
  TextureLayerBase::resize(layout.circle.box_rect);
}

void RadarEngine::clearData() {
  auto atr_buf_size = _peleng_count*_peleng_size*static_cast<qopengl_GLintptr>(sizeof(GLfloat));
  std::vector<GLfloat> attrs(static_cast<size_t>(_peleng_count*_peleng_size), 0.f);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glBufferData(GL_ARRAY_BUFFER, atr_buf_size, attrs.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _first_recieved_peleng = 0;
  _recieved_peleng_count = 0;

  clearTexture();
}

void RadarEngine::updateData(int offset, int count, GLfloat* amps) {
  qopengl_GLintptr buf_start = offset*_peleng_size*static_cast<qopengl_GLintptr>(sizeof(GLfloat));
  qopengl_GLintptr update_size = count*_peleng_size*static_cast<qopengl_GLintptr>(sizeof(GLfloat));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  glBufferSubData(GL_ARRAY_BUFFER, buf_start, update_size, amps);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // New last added peleng  
  if (_recieved_peleng_count == 0)
    _first_recieved_peleng = offset;
  _recieved_peleng_count += count;
}

QMatrix4x4 RadarEngine::getMVP(const State& state) {
  QMatrix4x4 modelTransform;
  modelTransform.setToIdentity();
  modelTransform.translate(state.center_shift.x(), state.center_shift.y());
  modelTransform.rotate(state.north_shift, QVector3D(0, 0, 1));

  QMatrix4x4 view;
  view.setToIdentity();
  view.translate(width() / 2.f, height() / 2.f);

  return projection()*view*modelTransform;
}

void RadarEngine::paint(const State& state, const Layout& layout) {
  Q_UNUSED(layout)

  if (QVector2D(_center_shift - state.center_shift).length() > 0.5f) {
    clearTexture();
    _center_shift = state.center_shift;
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
  glBindFramebuffer(GL_FRAMEBUFFER, fboId());

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);

  glViewport(0.f, 0.f, width(), height());

  glUseProgram(progId());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _palette->texture());

  glUniform1i(unifLoc(UNIF_TEXTURE), 0);
  glUniformMatrix4fv(unifLoc(UNIF_MVP_MATRIX), 1, GL_FALSE, getMVP(state).data());
  glUniform1f(unifLoc(UNIF_THREASHOLD), state.amp_threashold);

  glUniform1f(unifLoc(UNIF_PELENG_SIZE), _peleng_size);
  glUniform1f(unifLoc(UNIF_PELENG_COUNT), _peleng_count);

  glBindVertexArray(_vao_id);

  //glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  //glVertexAttribPointer(attrLoc(ATTR_POSITION), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)));
  //glEnableVertexAttribArray(attrLoc(ATTR_POSITION));
  //
  //glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_AMPLITUDE]);
  //glVertexAttribPointer(attrLoc(ATTR_AMPLITUDE), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)));
  //glEnableVertexAttribArray(attrLoc(ATTR_AMPLITUDE));
  //
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eab_id);

  if (first_peleng_to_draw <= last_peleng_to_draw) {
    drawPelengs(first_peleng_to_draw, last_peleng_to_draw);
  } else {
    drawPelengs(first_peleng_to_draw, _peleng_count - 1);
    drawPelengs(0, last_peleng_to_draw);
  }

  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

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

  int inds_to_draw = (last-first+1)*(2*_peleng_size+2);
  size_t inds_offset = static_cast<size_t>(first*(2*_peleng_size+2));

  glDrawElements(GL_TRIANGLE_STRIP, inds_to_draw, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(inds_offset * sizeof(GLuint)));
  //glDrawArrays(GL_POINTS, first*_peleng_size, (last-first+1)*_peleng_size);
}

