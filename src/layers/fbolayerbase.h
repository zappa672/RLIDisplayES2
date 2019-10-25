#ifndef FBOLAYERBASE_H
#define FBOLAYERBASE_H

#include <QSize>
#include <QObject>

#include "../common/layout.h"
#include "../common/state.h"

#include "layerbase.h"

namespace RLI {

  class FboLayerBase : public LayerBase {
    Q_OBJECT
  public:
    FboLayerBase(const QSize& size, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~FboLayerBase();

    inline const QSize&   fboSize()     const { return _fbo_size; }
    inline const GLuint&  textureId()   const { return _fbo_tex_id; }

    inline int            fboWidth()    const { return _fbo_size.width(); }
    inline int            fboHeight()   const { return _fbo_size.height(); }

  public slots:
    virtual void resizeTexture(Layout* layout) =0;
    virtual void updateTexture(const State& state) =0;
    virtual void clearTexture() =0;

  protected:
    inline const GLuint&  fboId()   const { return _fbo_id; }

    void clear(float r, float g, float b, float a, float d);
    virtual void resize(const QSize& size);

  private:
    GLuint _fbo_id;
    GLuint _fbo_tex_id;
    GLuint _depth_rbo_id;        
    QSize _fbo_size;
  };

}

#endif // FBOLAYERBASE_H
