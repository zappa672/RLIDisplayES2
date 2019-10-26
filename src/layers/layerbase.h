#ifndef LAYERBASE_H
#define LAYERBASE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>

#include "../common/state.h"
#include "../common/layout.h"

#include <QMap>
#include <QString>

namespace RLI {

  class LayerBase : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
  public:
    LayerBase(QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~LayerBase();

    virtual void paint(const State& state, const Layout& layout) =0;

  protected:
    void initShaderProgram(const QString& glsl_path, const QMap<QString, int>& attr_inds, const QMap<QString, int>& unif_inds);

    inline GLuint progId() const { return _program.programId(); /*_prog_id; */ }

    inline GLuint attrId(int index) const { return _attr_ids.value(index, 0); }
    inline GLint unifId(int index) const { return _unif_ids.value(index, -1); }

  private:
    GLuint loadShader(GLenum type, const char* filePath);

    QOpenGLShaderProgram _program { this };
    //GLint _prog_id = -1;
    QMap<int, GLuint> _attr_ids;
    QMap<int, GLint> _unif_ids;
  };

}

#endif // LAYERBASE_H
