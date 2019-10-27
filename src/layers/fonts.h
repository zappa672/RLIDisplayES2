#ifndef INFOFONTS_H
#define INFOFONTS_H

#include <QSize>
#include <QMap>

#include <QOpenGLFunctions>

namespace RLI {

  class Fonts : protected QOpenGLFunctions
  {
  public:
    Fonts(QOpenGLContext* context, const QString& dirPath);
    virtual ~Fonts();

    inline GLuint texture(const QString& tag)   { return _textures.value(tag, DEFAULT).first; }
    inline QSize  fontSize(const QString& tag)  { return _textures.value(tag, DEFAULT).second; }

  private:
    const std::pair<GLuint, QSize> DEFAULT { 0, QSize(0,0) };
    QMap<QString, std::pair<GLuint, QSize>> _textures;
  };

}

#endif // INFOFONTS_H
