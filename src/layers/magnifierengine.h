#ifndef MAGNIFIERENGINE_H
#define MAGNIFIERENGINE_H

#include <QString>
#include <QPoint>
#include <QSize>
#include <QMap>

#include "fbolayerbase.h"

namespace RLI {

  class MagnifierEngine : public FboLayerBase {
    Q_OBJECT

  public:
    MagnifierEngine(const State& state, const Layout& layout, GLuint amps_vbo_id, GLuint palette_tex_id, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~MagnifierEngine() override;

  public slots:
    void paint(const State& state, const Layout& layout) override;
    void resizeTexture(const Layout& layout) override;
    void clearTexture() override;

  private:
    void initBorderBuffers();
    void initRadarBuffers();

    void drawBorder();
    void drawPelengs(int pel_len, int pel_cnt, int min_pel, int min_rad);

    GLuint _amp_vbo_id;
    GLuint _pal_tex_id;

    // -----------------------------------------------

    enum SHADER_ATTRIBUTES {
      ATTR_POSITION  = 0
    , ATTR_AMPLITUDE = 1
    , ATTR_COUNT     = 2 } ;
    enum SHADER_UNIFORMS {
      UNIF_MVP         = 0
    , UNIF_COLOR       = 1
    , UNIF_TEXTURE     = 2
    , UNIF_THREASHOLD  = 3 } ;

    GLuint _vbo_ids_border[ATTR_COUNT];
    GLuint _vbo_ids_radar[ATTR_COUNT];
  };

}

#endif // MAGNIFIERENGINE_H
