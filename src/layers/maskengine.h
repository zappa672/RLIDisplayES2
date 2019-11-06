#ifndef MASKENGINE_H
#define MASKENGINE_H

#define CIRCLE_RAY_COUNT 720
#define MARK_RAY_COUNT 36

#include <QVector>
#include <QMap>

#include "texturelayerbase.h"

#include "fonts.h"

namespace RLI {

  class MaskEngine : public TextureLayerBase {
    Q_OBJECT
  public:
    MaskEngine(const State& state, const Layout& layout, Fonts* fonts, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~MaskEngine() override;

  public slots:
    void paint(const State& state, const Layout& layout) override;
    void resizeTexture(const Layout& layout) override;
    void clearTexture() override;

  private:
    void initBuffers();

    void initRectBuffers();
    void initLineBuffers();
    void initTextBuffers();
    void initHoleBuffers();

    void bindBuffers(GLuint* vbo_ids);
    void setBuffers(GLuint* vbo_ids, uint count, GLfloat* angles, GLfloat* chars, GLfloat* orders, GLfloat* shifts);

    Fonts* _fonts;

    bool _forceUpdate;

    Orientation _orientation;
    double  _north_shift;
    QPoint  _center_shift { -1, -1 };

    enum SHADER_ATTRIBUTES {
      ATTR_ANGLE    = 0
    , ATTR_CHAR_VAL = 1
    , ATTR_ORDER    = 2
    , ATTR_SHIFT    = 3
    , ATTR_COUNT    = 4 };
    enum SHADER_UNIFORMS {
      UNIF_MVP            = 0
    , UNIF_ANGLE_SHIFT    = 1
    , UNIF_CIRCLE_RADIUS  = 2
    , UNIF_CIRCLE_POS     = 3
    , UNIF_CURSOR_POS     = 4
    , UNIF_COLOR          = 5
    , UNIF_FONT_SIZE      = 6
    , UNIF_GLYPH_TEX      = 7 };

    GLuint _vbo_ids_mark   [ATTR_COUNT];

    GLuint _ind_vbo_id_text[2];
    GLuint _vbo_ids_text   [2][ATTR_COUNT];
    uint _text_point_count[2];

    GLuint _vbo_ids_hole   [ATTR_COUNT];
    uint _hole_point_count;
  };

}
#endif // MASKENGINE_H
