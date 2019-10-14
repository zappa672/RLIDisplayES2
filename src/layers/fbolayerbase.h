#ifndef FBOLAYERBASE_H
#define FBOLAYERBASE_H

#include <QSize>
#include <QObject>
#include <QOpenGLExtraFunctions>

#include "../common/layout.h"
#include "../common/state.h"

namespace RLI {

  class FboLayerBase : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
  public:
    FboLayerBase(const QSize& size, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~FboLayerBase();

    inline const QSize&   size()        const { return _fbo_size; }
    inline const GLuint&  textureId()   const { return _fbo_tex_id; }

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
