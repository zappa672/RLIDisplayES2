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

#include "datasources/radardatasource.h"
#include "datasources/shipdatasource.h"
#include "datasources/targetdatasource.h"

#include "layers/maskengine.h"
#include "layers/magnifierengine.h"
#include "layers/maskengine.h"
#include "layers/radar/radarengine.h"

namespace RLI {

  enum class DataSource : int
  { Radar   = 0
  , Ship    = 1
  , Target  = 2 };

  enum class SimpleLayer : int
  { Controls  = 0
  , Path      = 1
  , Targets   = 2 };

  enum class TextureLayer : int
  { Radar       = 0
  , Trail       = 1
  , Mask        = 2
  , Magnifier   = 3
  , Menu        = 4
  , Info        = 5 };

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

    inline RadarDataSource*   radarDS()   { return static_cast<RadarDataSource*>(_data_sources[DataSource::Radar]); }
    inline ShipDataSource*    shipDS()    { return static_cast<ShipDataSource*>(_data_sources[DataSource::Ship]); }
    inline TargetDataSource*  targetDS()  { return static_cast<TargetDataSource*>(_data_sources[DataSource::Target]); }

    inline RadarEngine* radarLayer() { return static_cast<RadarEngine*>(_tex_layers[TextureLayer::Radar]); }
    inline RadarEngine* trailLayer() { return static_cast<RadarEngine*>(_tex_layers[TextureLayer::Trail]); }
    inline MaskEngine* maskLayer() { return static_cast<MaskEngine*>(_tex_layers[TextureLayer::Mask]); }
    inline MagnifierEngine* magnifierLayer() { return static_cast<MagnifierEngine*>(_tex_layers[TextureLayer::Magnifier]); }

    int _timerId = -1;

    State _state;

    LayoutManager _layout_manager  { "layouts.xml" };
    inline const Layout& layout() { return _layout_manager.layout(); }

    QMap<DataSource, DataSourceBase*> _data_sources;

    Fonts* _fonts;

    QMap<SimpleLayer, LayerBase*> _simple_layers;
    QMap<TextureLayer, TextureLayerBase*> _tex_layers;

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
