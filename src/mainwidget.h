#ifndef SIMPLEOPENGLWIDGET_H
#define SIMPLEOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <QTimerEvent>

#include "common/state.h"
#include "common/layout.h"

#include "datasources/radardatasource.h"

#include "layers/radar/radarengine.h"

namespace RLI {

  class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
  {
    Q_OBJECT
  public:
    explicit MainWidget(QWidget *parent = nullptr);
    virtual ~MainWidget() override;

  protected slots:
    void timerEvent(QTimerEvent* e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

  private:
    void initProgram();

    void drawRect(const QRectF& rect, GLuint textureId);

    int _timerId = -1;

    State _state;
    LayoutManager _layout_manager  { "layouts.xml" };

    RadarDataSource* _ds_radar;
    \
    RadarEngine* _radarLayer;

    QMatrix4x4 _projection;
    QOpenGLShaderProgram* _program;
    enum { ATTR_POSITION = 0, ATTR_TEXCOORD = 1, ATTR_COUNT = 2 } ;    
    GLuint _vbo_ids[ATTR_COUNT];
    GLuint _attr_locs[ATTR_COUNT];
  };

}

#endif // SIMPLEOPENGLWIDGET_H
