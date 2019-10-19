#ifndef RADARENGINE_H
#define RADARENGINE_H

#include <map>
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

    void resizeData(int peleng_count, int peleng_size);
    void updateData(int offset, int count, GLfloat* amps);
    void clearData();

  private:
    void initBuffers();
    QMatrix4x4 getMVP(const State& state);
    void drawPelengs(int first, int last);

    // Layer vars
    QPoint  _center_shift { 0, 0 };
    int     _peleng_count;
    int     _peleng_size;

    int  _first_recieved_peleng;
    int  _recieved_peleng_count;

    // Palette
    RadarPalette* _palette;

    // OpenGL vars
    static const int ATTR_POSITION  = 0;
    static const int ATTR_AMPLITUDE = 1;
    static const int ATTR_COUNT     = 2;

    static const int UNIF_MVP_MATRIX    = 0;
    static const int UNIF_TEXTURE       = 1;
    static const int UNIF_THREASHOLD    = 2;
    static const int UNIF_PELENG_SIZE   = 3;
    static const int UNIF_PELENG_COUNT  = 4;

    GLuint _vao_id;               // Vertex array oobject id
    GLuint _eab_id;               // Element array buffer id
    GLuint _vbo_ids[ATTR_COUNT];  // Vertex buffer object id's (one per shader attribute)
  };

}

#endif // RADARENGINE_H
