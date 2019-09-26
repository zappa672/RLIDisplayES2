#ifndef RADARENGINE_H
#define RADARENGINE_H

#include <vector>

#include <QTime>
#include <QColor>
#include <QVector2D>

#include <QOpenGLShaderProgram>

#include "../../common/state.h"
#include "../../common/layout.h"

#include "radarpalette.h"

#include "../fbolayerbase.h"


namespace RLI {

  // Класс для отрисовки радарного круга
  class RadarEngine : public FboLayerBase {
    Q_OBJECT
  public:
    RadarEngine(const State& state, Layout* layout, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~RadarEngine() override;

    inline GLuint ampsVboId()     const { return _vbo_ids[ATTR_AMPLITUDE]; }
    inline GLuint paletteTexId()  const { return _palette->texture(); }

  public slots:
    void onBrightnessChanged(int br);

    void resizeTexture(Layout* layout) override;
    void updateTexture(const State& state) override;
    void clearTexture() override;

    void resizeData(const State& state);
    void updateData(int offset, int count, GLfloat* amps);
    void clearData();

  private:
    bool _has_data = false;

    void initShader();
    void initBuffers();

    void drawPelengs(int first, int last);

    // OpenGL vars
    QOpenGLShaderProgram* _program;

    enum { ATTR_POSITION = 0, ATTR_AMPLITUDE = 1, ATTR_COUNT = 2 } ;
    enum { UNIF_MVP_MATRIX    = 0
         , UNIF_TEXTURE       = 1
         , UNIF_THREASHOLD    = 2
         , UNIF_PELENG_LENGTH = 3
         , UNIF_PELENG_COUNT  = 4
         , UNIF_FBO_RADIUS    = 5
         , UNIF_NORTH_SHIFT   = 6
         , UNIF_COUNT         = 7 } ;

    GLuint _vbo_ids[ATTR_COUNT];
    int _attr_locs[ATTR_COUNT];
    int _unif_locs[UNIF_COUNT];
    GLuint _ind_vbo_id;

    int _peleng_count = 0;
    int _peleng_len   = 0;

    bool  _draw_circle;
    int  _last_drawn_peleng;
    int  _last_added_peleng;
    QPoint _center_shift { 0, 0 };

    // Palette
    RadarPalette* _palette;
  };

}

#endif // RADARENGINE_H
