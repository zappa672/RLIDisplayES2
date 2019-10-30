#ifndef SIMPLEOPENGLWIDGET_H
#define SIMPLEOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>

#include <QTimerEvent>

#include "common/state.h"
#include "common/layout.h"

#include "datasources/radardatasource.h"

#include "layers/fonts.h"

#include "layers/radar/radarengine.h"
#include "layers/maskengine.h"
#include "layers/magnifierengine.h"

namespace RLI {

  class MainWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
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
    void debugInfo();

    void updateLayers();
    void paintLayers();

    void initProgram();
    void initBuffers();

    void drawRect(const QRect& rect, GLuint textureId);

    int _timerId = -1;

    State _state;

    LayoutManager _layout_manager  { "layouts.xml" };
    inline const Layout& layout() { return _layout_manager.layout(); }

    RadarDataSource* _ds_radar;

    Fonts* _fonts;
    \
    RadarEngine* _lr_radar;
    RadarEngine* _lr_trail;
    MaskEngine* _lr_mask;
    MagnifierEngine* _lr_magn;

    QMatrix4x4 _projection;

    QOpenGLShaderProgram _program { this };
    enum { ATTR_POSITION = 0
         , ATTR_TEXCOORD = 1
         , ATTR_COUNT = 2 } ;

    GLuint _vao_id;
    GLuint _vbo_ids[ATTR_COUNT];
    GLuint _attr_locs[ATTR_COUNT];
  };

}

#endif // SIMPLEOPENGLWIDGET_H
