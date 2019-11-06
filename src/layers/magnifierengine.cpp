#include "magnifierengine.h"

using namespace RLI;

MagnifierEngine::MagnifierEngine(const State&, const Layout& layout, QOpenGLContext* context, QObject* parent)
  : TextureLayerBase(layout.magnifier.geometry, context, parent) {

  initializeOpenGLFunctions();

  glGenBuffers(ATTR_COUNT, _vbo_ids_border);
  glGenBuffers(ATTR_COUNT, _vbo_ids_radar);

  static const QMap<QString, int> attr_inices
  { { "a_position",  ATTR_POSITION  }
  , { "a_amplitude", ATTR_AMPLITUDE } };

  static const QMap<QString, int> unif_inices
  { { "mvp_matrix", UNIF_MVP        }
  , { "color",      UNIF_COLOR      }
  , { "texture",    UNIF_TEXTURE    }
  , { "threashold", UNIF_THREASHOLD } };

  initShaderProgram(":/shaders/magn", attr_inices, unif_inices);

  initRadarBuffers();
  initBorderBuffers();
}

MagnifierEngine::~MagnifierEngine() {
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_border);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_radar);
}

void MagnifierEngine::resizeTexture(const Layout& layout) {
  TextureLayerBase::resize(layout.magnifier.geometry);
}

void MagnifierEngine::clearTexture() {
  TextureLayerBase::clear(1.f, 1.f, 1.f, 0.f, 0.f);
}

void MagnifierEngine::paint(const State& s, const Layout&) {
  if (_amp_vbo_id == -1 || _pal_tex_id == -1)
    return;

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());

  glDisable(GL_DEPTH_TEST);

  glViewport(0.f, 0.f, width(), height());

  glUseProgram(progId());

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, width(), height(), 0.f, -1.f, 1.f);

  glUniformMatrix4fv(unifLoc(UNIF_MVP), 1, GL_FALSE, projection.data());
  glUniform1i(unifLoc(UNIF_TEXTURE), 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(_pal_tex_id));

  drawPelengs(s.peleng_size, s.peleng_count, s.magn_min_peleng, s.magn_min_rad);
  drawBorder();

  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MagnifierEngine::drawPelengs(int pel_len, int pel_cnt, int min_pel, int min_rad) {
  glUniform4f(unifLoc(UNIF_COLOR), 0.0f, 0.0f, 0.0f, 1.0f);
  glUniform1f(unifLoc(UNIF_THREASHOLD), 1.f);

  for (int i = 0; i < width() - 2; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_radar[ATTR_POSITION]);
    glVertexAttribPointer(attrLoc(ATTR_POSITION), 2, GL_FLOAT, GL_FALSE, 0, (void*) (2 * (height()-2) * i * sizeof(GLfloat)));
    glEnableVertexAttribArray(attrLoc(ATTR_POSITION));

    int amp_shift = ((min_pel + i) % pel_cnt) * pel_len + min_rad;

    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(_amp_vbo_id));
    glVertexAttribPointer(attrLoc(ATTR_AMPLITUDE), 1, GL_FLOAT, GL_FALSE, 0, (void*) (amp_shift * sizeof(GLfloat)));
    glEnableVertexAttribArray(attrLoc(ATTR_AMPLITUDE));

    glDrawArrays(GL_POINTS, 0, (height()-2));
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MagnifierEngine::drawBorder() {
  glUniform4f(unifLoc(UNIF_COLOR), 0.0f, 1.0f, 0.0f, 1.0f);
  glUniform1f(unifLoc(UNIF_THREASHOLD), 255.f);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_border[ATTR_POSITION]);
  glVertexAttribPointer(attrLoc(ATTR_POSITION), 2, GL_FLOAT, GL_FALSE, 0, (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(attrLoc(ATTR_POSITION));

  glVertexAttrib1f(attrLoc(ATTR_AMPLITUDE), 0.f);
  glDisableVertexAttribArray(attrLoc(ATTR_AMPLITUDE));

  glLineWidth(1.f);
  glDrawArrays(GL_LINE_LOOP, 0, 4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MagnifierEngine::initBorderBuffers() {
  GLfloat positions[] { 0.5f          , 0.5f
                      , 0.5f          , height()-0.5f
                      , width()-0.5f  , height()-0.5f
                      , width()-0.5f  , 0.5f                 };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_border[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), positions, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MagnifierEngine::initRadarBuffers() {
  std::vector<GLfloat> positions;

  for (float i = 1.5; i < width() - 1; i++) {
    for (float j = 1.5; j < height() - 1; j++) {
      positions.push_back(i);
      positions.push_back(j);
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_radar[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, static_cast<uint>(positions.size())*sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
