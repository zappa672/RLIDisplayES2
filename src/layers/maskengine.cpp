#include "maskengine.h"

#include "../common/properties.h"

#include <QTextCodec>

#include <cmath>

static double const PI = acos(-1);

using namespace RLI;

MaskEngine::MaskEngine(const State& state, const Layout& layout, Fonts* fonts, QOpenGLContext* context, QObject* parent)
    : FboLayerBase(QRect(QPoint(0,0), layout.size), context, parent) {

  initializeOpenGLFunctions();

  glGenBuffers(ATTR_COUNT, _vbo_ids_mark);
  glGenBuffers(2, _ind_vbo_id_text);
  glGenBuffers(ATTR_COUNT, _vbo_ids_text[0]);
  glGenBuffers(ATTR_COUNT, _vbo_ids_text[1]);
  glGenBuffers(ATTR_COUNT, _vbo_ids_hole);

  _fonts = fonts;

  _text_point_count[0] = 0;
  _text_point_count[1] = 0;
  _hole_point_count = 362;

  static const QMap<QString, int> attr_inices
  { { "angle",    ATTR_ANGLE    }
  , { "char_val", ATTR_CHAR_VAL }
  , { "order",    ATTR_ORDER    }
  , { "shift",    ATTR_SHIFT    } };

  static const QMap<QString, int> unif_inices
  { { "mvp_matrix",     UNIF_MVP           }
  , { "angle_shift",    UNIF_ANGLE_SHIFT   }
  , { "circle_radius",  UNIF_CIRCLE_RADIUS }
  , { "circle_pos",     UNIF_CIRCLE_POS    }
  , { "cursor_pos",     UNIF_CURSOR_POS    }
  , { "color",          UNIF_COLOR         }
  , { "font_size",      UNIF_FONT_SIZE     }
  , { "glyph_tex",      UNIF_GLYPH_TEX     } };

  initShaderProgram(":/shaders/mask", attr_inices, unif_inices);
  initBuffers();

  paint(state, layout);

  _orientation = state.orientation;
  _north_shift = state.north_shift;
  _center_shift = state.center_shift;
}

MaskEngine::~MaskEngine() {
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_mark);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_text[0]);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_text[1]);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_hole);
  glDeleteBuffers(2, _ind_vbo_id_text);
}

void MaskEngine::resizeTexture(const Layout& layout) {
  FboLayerBase::resize(QRect(QPoint(0,0), layout.size));
  _forceUpdate = true;
}

void MaskEngine::clearTexture() {
  FboLayerBase::clear(1.f, 1.f, 1.f, 0.f, 0.f);
}

void MaskEngine::paint(const State& state, const Layout& layout)  {
  if ( !_forceUpdate
    && fabs(state.north_shift - _north_shift) < 0.5
    && QVector2D(_center_shift - state.center_shift).length() < 1.f
    && _orientation == state.orientation )
    return;

  _north_shift = state.north_shift;
  _center_shift = state.center_shift;
  _orientation = state.orientation;

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());

  glViewport(0.f, 0.f, width(), height());

  glUseProgram(progId());

  glClearColor(0.12f, 0.13f, 0.10f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  QMatrix4x4 projection;
  projection.setToIdentity();
  projection.ortho(0.f, width(), 0.f, height(), -1.f, 1.f);


  // Set uniforms
  // ---------------------------------------------------------------------
  glUniformMatrix4fv(unifLoc(UNIF_MVP), 1, GL_FALSE, projection.data());
  glUniform1f(unifLoc(UNIF_ANGLE_SHIFT), static_cast<float>(_north_shift));
  glUniform1f(unifLoc(UNIF_CIRCLE_RADIUS), layout.circle.radius);
  glUniform2f(unifLoc(UNIF_CIRCLE_POS), layout.circle.center.x(), layout.circle.center.y());
  glUniform4f(unifLoc(UNIF_COLOR), 0.f, 1.f, 0.f, 1.f);
  glUniform2f(unifLoc(UNIF_CURSOR_POS), static_cast<float>(layout.circle.center.x() + _center_shift.x())
                                      , static_cast<float>(layout.circle.center.y() + _center_shift.y()));
  // ---------------------------------------------------------------------

  // Draw line marks
  // ---------------------------------------------------------------------
  bindBuffers(_vbo_ids_mark);
  glLineWidth(1);
  glDrawArrays(GL_LINES, 0, 2*360);
  // ---------------------------------------------------------------------

  // Draw text mark
  // ---------------------------------------------------------------------
  QSize font_size = _fonts->fontSize(layout.circle.font);
  GLuint tex_id = _fonts->texture(layout.circle.font);

  glUniform2f(unifLoc(UNIF_FONT_SIZE), font_size.width(), font_size.height());
  glUniform1f(unifLoc(UNIF_GLYPH_TEX), 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  if (state.orientation == Orientation::NORTH) {
    bindBuffers(_vbo_ids_text[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text[0]);
    glDrawElements(GL_TRIANGLES, 3*(_text_point_count[0]/2), GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0 * sizeof(GLuint)));
  } else {
    bindBuffers(_vbo_ids_text[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text[1]);
    glDrawElements(GL_TRIANGLES, 3*(_text_point_count[1]/2), GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0 * sizeof(GLuint)));
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, 0);
  // ---------------------------------------------------------------------

  glDisable(GL_BLEND);

  // Draw hole
  // ---------------------------------------------------------------------
  glUniform4f(unifLoc(UNIF_COLOR), 1.f, 1.f, 1.f, 0.f);
  bindBuffers(_vbo_ids_hole);
  glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<int>(_hole_point_count+2));
  // ---------------------------------------------------------------------

  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _forceUpdate = false;
}



void MaskEngine::initBuffers() {
  initLineBuffers();
  initTextBuffers();
  initHoleBuffers();
  initRectBuffers();
}

void MaskEngine::setBuffers(GLuint* vbo_ids, uint count, GLfloat* angles, GLfloat* chars, GLfloat* orders, GLfloat* shifts) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), angles, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), chars, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), orders, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_SHIFT]);
  glBufferData(GL_ARRAY_BUFFER, count*sizeof(GLfloat), shifts, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MaskEngine::bindBuffers(GLuint* vbo_ids) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_ANGLE]);
  glVertexAttribPointer(attrLoc(ATTR_ANGLE), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_ANGLE));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_CHAR_VAL]);
  glVertexAttribPointer(attrLoc(ATTR_CHAR_VAL), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_CHAR_VAL));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_ORDER]);
  glVertexAttribPointer(attrLoc(ATTR_ORDER), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[ATTR_SHIFT]);
  glVertexAttribPointer(attrLoc(ATTR_SHIFT), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_SHIFT));
}


void MaskEngine::initRectBuffers() {

}

void MaskEngine::initHoleBuffers() {
  QVector<GLfloat> angle;
  QVector<GLfloat> chars, order, shift;

  angle.push_back(0);
  order.push_back(0);
  chars.push_back(0);
  shift.push_back(0);

  for (uint a = 0; a < _hole_point_count; a++) {
    angle.push_back((a * 360.f) / _hole_point_count);
    order.push_back(1);
    chars.push_back(0);
    shift.push_back(0);
  }

  angle.push_back(angle.at(1));
  order.push_back(1);
  chars.push_back(0);
  shift.push_back(0);

  setBuffers(_vbo_ids_hole, _hole_point_count+2, angle.data(), chars.data(), order.data(), shift.data());
}

void MaskEngine::initLineBuffers() {
  GLfloat angle[2*360];
  GLfloat chars[2*360];
  GLfloat order[2*360];
  GLfloat shift[2*360];

  for (int a = 0; a < 360; a++) {
    angle[2*a+0] = angle[2*a+1] = a;
    order[2*a+0] = 0;
    order[2*a+1] = 1;
    chars[2*a+0] = chars[2*a+1] = 0;

    char s = 2;
    if (a%5  == 0) s =  4;
    if (a%10 == 0) s =  7;
    if (a%30 == 0) s = 12;

    shift[2*a+0] = shift[2*a+1] = s;
  }

  setBuffers(_vbo_ids_mark, 2*360, angle, chars, order, shift);
}

void MaskEngine::initTextBuffers() {
  QVector<GLfloat> angles[2];
  QVector<GLfloat> chars[2];
  QVector<GLfloat> orders[2];
  QVector<GLfloat> shifts[2];

  QTextEncoder* encoder = QTextCodec::codecForName("cp866")->makeEncoder();

  for (int i = 0; i < 360; i += 10) {    
    QByteArray tm[2] { encoder->fromUnicode(QString::number(i))
                     , encoder->fromUnicode(QString::number(i > 180 ? 360-i : i)) };

    for (int j = 0; j < 2; j++) {
      for (int l = 0; l < tm[j].size(); l++) {
        for (int k = 0; k < 4; k++) {
          angles[j].push_back(i);
          chars[j].push_back(tm[j][l]);
          orders[j].push_back(k);
          shifts[j].push_back(l);
        }
      }
    }
  }

  delete encoder;

  _text_point_count[0] = static_cast<uint>(orders[0].size());
  setBuffers(_vbo_ids_text[0], _text_point_count[0], angles[0].data(), chars[0].data(), orders[0].data(), shifts[0].data());
  _text_point_count[1] = static_cast<uint>(orders[1].size());
  setBuffers(_vbo_ids_text[1], _text_point_count[1], angles[1].data(), chars[1].data(), orders[1].data(), shifts[1].data());


  std::vector<GLuint> draw_indices[2];

  for (int k = 0; k < 2; k++) {
    for (uint i = 0; i < _text_point_count[k]; i += 4) {
      draw_indices[k].push_back(i);
      draw_indices[k].push_back(i+1);
      draw_indices[k].push_back(i+2);
      draw_indices[k].push_back(i);
      draw_indices[k].push_back(i+2);
      draw_indices[k].push_back(i+3);
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*_text_point_count[0]*sizeof(GLuint), draw_indices[0].data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_vbo_id_text[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*_text_point_count[1]*sizeof(GLuint), draw_indices[1].data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
