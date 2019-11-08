#ifndef SIMPLEOPENGLWIDGET_H
#define SIMPLEOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>

#include <QTimerEvent>
#include <QKeyEvent>

#include <QQueue>
#include <QSet>

#include "common/state.h"
#include "common/layout.h"

#include "datasources/datasourcebase.h"

#include "datasources/radardatasource.h"
#include "datasources/shipdatasource.h"
#include "datasources/targetdatasource.h"

#include "layers/fonts.h"

#include "layers/maskengine.h"
#include "layers/magnifierengine.h"
#include "layers/maskengine.h"
#include "layers/radar/radarengine.h"
#include "layers/info/infoengine.h"
#include "layers/info/menuengine.h"

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
  , Menu        = 4 };

  class MainWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
  {
    Q_OBJECT
  public:
    explicit MainWidget(QWidget *parent = nullptr);
    virtual ~MainWidget() override;

  protected slots:
    void timerEvent(QTimerEvent* e) override;

    void keyReleaseEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

  private:
    float frameRate();
    QQueue<QDateTime> _frameTimes;

    void debugInfo();

    void initProgram();
    void initBuffers();

    void drawRect(const QRect& rect, GLuint textureId);

    inline RadarDataSource*   radarDS()   { return static_cast<RadarDataSource*>(_data_sources[DataSource::Radar]);   }
    inline ShipDataSource*    shipDS()    { return static_cast<ShipDataSource*>(_data_sources[DataSource::Ship]);     }
    inline TargetDataSource*  targetDS()  { return static_cast<TargetDataSource*>(_data_sources[DataSource::Target]); }

    inline RadarEngine*     radarLayer()      { return static_cast<RadarEngine*>(_tex_layers[TextureLayer::Radar]);         }
    inline RadarEngine*     trailLayer()      { return static_cast<RadarEngine*>(_tex_layers[TextureLayer::Trail]);         }
    inline MaskEngine*      maskLayer()       { return static_cast<MaskEngine*>(_tex_layers[TextureLayer::Mask]);           }
    inline MenuEngine*      menuLayer()       { return static_cast<MenuEngine*>(_tex_layers[TextureLayer::Menu]);           }
    inline MagnifierEngine* magnifierLayer()  { return static_cast<MagnifierEngine*>(_tex_layers[TextureLayer::Magnifier]); }

    inline InfoEngine*      infoLayer()       { return _info_layer; }

    int _timerId = -1;
    QSet<int> _pressed_keys;


    State _state;

    LayoutManager _layout_manager  { "layouts.xml" };
    inline const Layout& layout() { return _layout_manager.layout(); }

    std::map<DataSource, DataSourceBase*> _data_sources;

    Fonts* _fonts;

    InfoEngine* _info_layer;
    std::map<SimpleLayer, LayerBase*> _simple_layers;
    std::map<TextureLayer, TextureLayerBase*> _tex_layers;

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
