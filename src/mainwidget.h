#ifndef SIMPLEOPENGLWIDGET_H
#define SIMPLEOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>

#include <QTimerEvent>

#include "common/state.h"
#include "common/layout.h"

#include "datasources/datasourcebase.h"

#include "layers/fonts.h"

#include "layers/layerbase.h"
#include "layers/fbolayerbase.h"

namespace RLI {

  enum class DataSources : int
  { Radar   = 0
  , Ship    = 1
  , Targets = 2 };

  enum class SimpleLayers : int
  { Controls  = 0
  , Path      = 1
  , Targets   = 2 };

  enum class FboLayers : int
  { Radar     = 0
  , Trail     = 1
  , Mask      = 2
  , Magnifier = 3
  , Info      = 4 };

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

    void initProgram();
    void initBuffers();

    void drawRect(const QRect& rect, GLuint textureId);

    int _timerId = -1;

    State _state;

    LayoutManager _layout_manager  { "layouts.xml" };
    inline const Layout& layout() { return _layout_manager.layout(); }

    QMap<DataSources, DataSourceBase*> _data_sources;

    Fonts* _fonts;

    QMap<SimpleLayers, LayerBase*> _simple_layers;
    QMap<FboLayers, FboLayerBase*> _fbo_layers;

    QMatrix4x4 _projection;

    QOpenGLShaderProgram _program;
    enum { ATTR_POSITION = 0
         , ATTR_TEXCOORD = 1
         , ATTR_COUNT = 2 } ;

    GLuint _vao_id;
    GLuint _vbo_ids[ATTR_COUNT];
    GLuint _attr_locs[ATTR_COUNT];
  };

}

#endif // SIMPLEOPENGLWIDGET_H
