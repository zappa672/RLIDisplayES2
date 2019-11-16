#ifndef CONTROLSENGINE_H
#define CONTROLSENGINE_H

#include <QObject>
#include <QPoint>
#include <QColor>

#include "layerbase.h"

#include "../common/state.h"
#include "../common/layout.h"


namespace RLI {

  class ControlsEngine : public LayerBase {
    Q_OBJECT
  public:
    explicit ControlsEngine(QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~ControlsEngine() override;

    void paint(const State &state, const Layout &layout) override;

  private:
    const QColor COLOR_PAR_LINES     { 255, 255, 255, 255 };
    const QColor COLOR_CAPT_AREA     { 255, 255,   0, 255 };
    const QColor COLOR_MAGNIFIER     {   0,   0, 255, 255 };
    const QColor COLOR_CIRCLES       { 203,  67,  69, 255 };
    const QColor COLOR_CURSOR        { 255,   0, 255, 255 };
    const QColor COLOR_VN_CU         { 255, 192,  26, 255 };
    const QColor COLOR_CU            {   0, 255,   0, 255 };
    const QColor COLOR_VN_P          { 255, 254, 255, 255 };
    const QColor COLOR_VD            { 255, 255, 255, 255 };
    const QColor COLOR_COURSE_MARKER { 255,   0,   0, 255 };

    const size_t CIRCLE_RESOLUTION = 4096;

    void initCursorBuffers();
    void initCircleBuffers();

    void drawCursor(const QColor& col);

    void drawCircleSegment(const QColor& col, GLfloat radius, GLfloat min_angle = 0.f, GLfloat max_angle = 360.f);
    void drawRaySegment(const QColor& col, GLfloat angle, GLfloat min_radius = 0.f, GLfloat max_radius = 2048.f, GLfloat shift = 0.f);

    // -------------------------------------------
    enum SHADER_ATTRIBUTES
    { ATTR_ANGLE  = 0
    , ATTR_RADIUS = 1
    , ATTR_COUNT  = 2 };
    enum SHADER_UNIFORMS
    { UNIF_MVP    = 0
    , UNIF_SHIFT  = 1
    , UNIF_COLOR  = 2
    , UNIF_COUNT  = 3 };

    GLuint _vbo_ids_cursor[ATTR_COUNT];
    GLuint _vbo_ids_circle[ATTR_COUNT];
    GLuint _vbo_ids_ray[ATTR_COUNT];
  };

};
#endif // CONTROLSENGINE_H
