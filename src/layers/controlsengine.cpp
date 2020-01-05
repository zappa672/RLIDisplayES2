#include "controlsengine.h"
#include "../common/properties.h"

#include <vector>
#include <cmath>

#include "../common/math.h"

using namespace RLI;

ControlsEngine::ControlsEngine(QOpenGLContext* context, QObject* parent) : LayerBase(context, parent) {
  glGenBuffers(ATTR_COUNT, _vbo_ids_cursor);
  glGenBuffers(ATTR_COUNT, _vbo_ids_circle);
  glGenBuffers(ATTR_COUNT, _vbo_ids_ray);

  static const QMap<QString, int> attr_inices
  { { "angle",  ATTR_ANGLE  }
  , { "radius", ATTR_RADIUS } };

  static const QMap<QString, int> unif_inices
  { { "mvp_matrix", UNIF_MVP    }
  , { "color",      UNIF_COLOR  }
  , { "shift",      UNIF_SHIFT  } };

  initShaderProgram(":/shaders/ctrl", attr_inices, unif_inices);

  initCursorBuffers();
  initCircleBuffers();
}

ControlsEngine::~ControlsEngine() {
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_cursor);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_circle);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids_ray);
}

void ControlsEngine::paint(const State &state, const Layout &layout) {
  glUseProgram(progId());

  QMatrix4x4 transform0;
  transform0.setToIdentity();
  transform0.translate( layout.circle.center.x() + state.center_shift.x()
                      , layout.circle.center.y() + state.center_shift.y()
                      , 0);

  QMatrix4x4 transform;
  transform.setToIdentity();

  if (state.state == WidgetState::ROUTE_EDITION) {
    QPointF tr = Math::coords_to_pos(state.ship_position, state.visir_center_pos, QPoint(0,0), state.chart_scale);
    transform.translate(static_cast<GLfloat>(tr.x()), static_cast<GLfloat>(tr.y()), 0.f);
  }

  glUniformMatrix4fv(unifLoc(UNIF_MVP), 1, GL_FALSE, (state.projection*transform0*transform).data());

  // Визир дальности
  drawCircleSegment(COLOR_VD,  static_cast<float>(state.vd));

  // Distance rings
  if (state.show_circles) {
    for (float rad = 80; rad < 560; rad += 80)
      drawCircleSegment(COLOR_CIRCLES,  rad);
  }
  // ----------------------

  // Direction rays
  drawRaySegment(COLOR_VN_CU, static_cast<float>(state.vn_cu));
  drawRaySegment(COLOR_VN_P , static_cast<float>(state.vn_p));



  // ----------------------

  // Parallel lines
  if (state.show_parallel) {
    drawRaySegment(COLOR_PAR_LINES, static_cast<float>(state.vn_p), -2048.f, 2048.f,  static_cast<float>(state.vd));
    drawRaySegment(COLOR_PAR_LINES, static_cast<float>(state.vn_p), -2048.f, 2048.f, -static_cast<float>(state.vd));
  }
  // ----------------------

  // Capture zone
  drawRaySegment   (COLOR_CAPT_AREA,  280.f,   48.f,  112.f);
  drawRaySegment   (COLOR_CAPT_AREA,  340.f,   48.f,  112.f);
  drawCircleSegment(COLOR_CAPT_AREA,   48.f,  280.f,  340.f);
  drawCircleSegment(COLOR_CAPT_AREA,  112.f,  280.f,  340.f);
  // ----------------------

  // Magnifier zone
  float magn_min_angle = static_cast<float>((state.magn_min_peleng / 4096.0) * 360.0 + state.north_shift);
  float magn_max_angle = static_cast<float>(((state.magn_min_peleng + state.magn_width) / 4096.0) * 360.0 + state.north_shift);

  drawRaySegment   (COLOR_MAGNIFIER, magn_min_angle,  state.magn_min_rad,  state.magn_min_rad + state.magn_height);
  drawRaySegment   (COLOR_MAGNIFIER, magn_max_angle,  state.magn_min_rad,  state.magn_min_rad + state.magn_height);
  drawCircleSegment(COLOR_MAGNIFIER, state.magn_min_rad  ,  magn_min_angle,  magn_max_angle);
  drawCircleSegment(COLOR_MAGNIFIER, state.magn_min_rad + state.magn_height ,  magn_min_angle,  magn_max_angle);
  // ----------------------

  // Cursor
  QPointF cusor_shift = state.cursor_pos - state.center_shift;
  transform.setToIdentity();
  transform.translate(static_cast<GLfloat>(cusor_shift.x()), static_cast<GLfloat>(cusor_shift.y()), 0.f);

  glUniformMatrix4fv(unifLoc(UNIF_MVP), 1, GL_FALSE, (state.projection*transform0*transform).data());

  drawCursor(COLOR_CURSOR);
  // ----------------------


  // Course marker
  glLineWidth(2.0f);
  double rad = layout.circle.radius - 6;
  double phi = Math::rads(state.course_mark_angle);
  QPointF tr_cm(rad * std::sin(phi), -rad * std::cos(phi));

  transform.setToIdentity();
  transform.translate( static_cast<GLfloat>(tr_cm.x()), static_cast<GLfloat>(tr_cm.y()), 0.f);

  glUniformMatrix4fv(unifLoc(UNIF_MVP), 1, GL_FALSE, (state.projection*transform0*transform).data());

  drawRaySegment(COLOR_COURSE_MARKER, static_cast<GLfloat>(state.course_mark_angle-45), 0, 16, 0);
  drawRaySegment(COLOR_COURSE_MARKER, static_cast<GLfloat>(state.course_mark_angle   ), 0, 16, 0);
  drawRaySegment(COLOR_COURSE_MARKER, static_cast<GLfloat>(state.course_mark_angle+45), 0, 16, 0);
  // ----------------------


  glUseProgram(0);
}


void ControlsEngine::initCursorBuffers() {
  GLfloat angs[] { 0.f, 180.f, 90.f, 270.f };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_cursor[ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat), angs, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::initCircleBuffers() {
  std::vector<GLfloat> angs;

  for (size_t i = 0; i < CIRCLE_RESOLUTION; i++)
    angs.push_back( (static_cast<float>(i) / CIRCLE_RESOLUTION) * 360.f );
  angs.push_back(0);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_circle[ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, angs.size()*sizeof(GLfloat), angs.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void ControlsEngine::drawCursor(const QColor& col) {
  glUniform1f(unifLoc(UNIF_SHIFT), 0.0);
  glUniform4f(unifLoc(UNIF_COLOR), col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_cursor[ATTR_ANGLE]);
  glVertexAttribPointer(attrLoc(ATTR_ANGLE), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_ANGLE));

  glVertexAttrib1f(attrLoc(ATTR_RADIUS), 5.f);
  glDisableVertexAttribArray(attrLoc(ATTR_RADIUS));

  glLineWidth(2.f);
  glDrawArrays(GL_LINES, 0, 4);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::drawCircleSegment(const QColor& col, GLfloat radius, GLfloat min_angle, GLfloat max_angle) {
  glUniform1f(unifLoc(UNIF_SHIFT), 0.0);
  glUniform4f(unifLoc(UNIF_COLOR), col.redF(), col.greenF(), col.blueF(), col.alphaF());

#if !(defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0))
  glPointSize(1.f);
#endif

  glVertexAttrib1f(attrLoc(ATTR_RADIUS), radius);
  glDisableVertexAttribArray(attrLoc(ATTR_RADIUS));

  int frst_element = (int((min_angle / 360) * CIRCLE_RESOLUTION) + CIRCLE_RESOLUTION) % CIRCLE_RESOLUTION;
  int last_element = (int((max_angle / 360) * CIRCLE_RESOLUTION) + CIRCLE_RESOLUTION) % CIRCLE_RESOLUTION;

  if (last_element == 0)
    last_element = CIRCLE_RESOLUTION;

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_circle[ATTR_ANGLE]);
  glVertexAttribPointer(attrLoc(ATTR_ANGLE), 1, GL_FLOAT, GL_FALSE, 0, (void*)(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(attrLoc(ATTR_ANGLE));

  if (last_element > frst_element) {
    glDrawArrays(GL_POINTS, frst_element, last_element - frst_element);
  } else {
    glDrawArrays(GL_POINTS, frst_element, CIRCLE_RESOLUTION - frst_element);
    glDrawArrays(GL_POINTS, 0, last_element);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ControlsEngine::drawRaySegment(const QColor& col, GLfloat angle, GLfloat min_radius, GLfloat max_radius, GLfloat shift) {
  glUniform1f(unifLoc(UNIF_SHIFT), shift);
  glUniform4f(unifLoc(UNIF_COLOR), col.redF(), col.greenF(), col.blueF(), col.alphaF());

  GLfloat angs[] { angle, angle };
  GLfloat rads[] { min_radius, max_radius };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_ray[ATTR_ANGLE]);
  glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat), angs, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_ANGLE), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_ANGLE));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids_ray[ATTR_RADIUS]);
  glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat), rads, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_RADIUS), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_RADIUS));

  glLineWidth(1.f);
  glDrawArrays(GL_LINES, 0, 2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
