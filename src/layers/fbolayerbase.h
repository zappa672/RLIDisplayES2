#ifndef FBOLAYERBASE_H
#define FBOLAYERBASE_H

#include <QObject>
#include <QOpenGLFunctions>

#include <QSize>

namespace RLI {

  class FboLayerBase : public QObject, protected QOpenGLFunctions {
    Q_OBJECT
  public:
    FboLayerBase(const QSize& size, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~FboLayerBase();

    inline const QSize&   size()        const { return _fbo_size; }
    inline const GLuint&  textureId()   const { return _fbo_tex_id; }

    virtual void update();
    virtual void clear();

    void resize(const QSize& size);

  private:
    GLuint _fbo_id;
    GLuint _fbo_tex_id;
    GLuint _depth_rbo_id;
    QSize _fbo_size;
  };

}

#endif // FBOLAYERBASE_H
