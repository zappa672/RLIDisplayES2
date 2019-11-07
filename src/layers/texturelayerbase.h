#ifndef TEXTURELAYERBASE_H
#define TEXTURELAYERBASE_H

#include <QRect>
#include <QMatrix4x4>

#include "../common/layout.h"
#include "layerbase.h"

namespace RLI {

  class TextureLayerBase : public LayerBase {
    Q_OBJECT
  public:
    TextureLayerBase(const QRect& size, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~TextureLayerBase();

    inline const QRect&   geometry()    const { return _fbo_rect; }

    inline       QSize    size()    const { return _fbo_rect.size(); }
    inline const GLuint&  texId()   const { return _fbo_tex_id; }

    inline int            width()   const { return _fbo_rect.width(); }
    inline int            height()  const { return _fbo_rect.height(); }

  public slots:
    virtual void paint(const State& state, const Layout& layout) =0;
    virtual void resizeTexture(const Layout& layout) =0;
    virtual void clearTexture() =0;

  protected:
    inline const QMatrix4x4&  projection()  const { return _projection; }
    inline const GLuint&      fboId()       const { return _fbo_id; }

    void clear(float r, float g, float b, float a, float d);
    void resize(const QRect& size);

  private:
    QMatrix4x4 _projection;

    GLuint _fbo_id;
    GLuint _fbo_tex_id;
    GLuint _depth_rbo_id;        
    QRect _fbo_rect;
  };

}

#endif // TEXTURELAYERBASE_H
