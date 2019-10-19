#ifndef LAYERBASE_H
#define LAYERBASE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

#include <QSet>
#include <QMap>
#include <QString>

namespace RLI {

  class LayerBase : protected QOpenGLExtraFunctions {
  public:
    LayerBase(QOpenGLContext* context);
    virtual ~LayerBase();

  protected:
    void initShaderProgram(const QString& glsl_path, const QMap<QString, int>& attr_inds, const QMap<QString, int>& unif_inds);

    inline GLuint progId() const { return _program->programId(); /*_prog_id; */ }

    inline GLuint attrId(int index) const { return _attr_ids.value(index, 0); }
    inline GLint unifId(int index) const { return _unif_ids.value(index, -1); }

  private:
    GLuint loadShader(GLenum type, const char* filePath);

    QOpenGLShaderProgram* _program;
    //GLint _prog_id = -1;
    QMap<int, GLuint> _attr_ids;
    QMap<int, GLint> _unif_ids;
  };

}

#endif // LAYERBASE_H
