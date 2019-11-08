#include "fonts.h"

#include <QPixmap>
#include <QDebug>
#include <QDir>
#include <QDateTime>

#include <QGLWidget>

using namespace RLI;

Fonts::Fonts(QOpenGLContext* context, const QString& dirPath) : QOpenGLFunctions(context) {
  initializeOpenGLFunctions();

  QDir dir(dirPath);
  dir.setNameFilters(QStringList() << "*.png");

  for (QString fileName : dir.entryList()) {
    QString tag = fileName.replace(".png", "");

    QImage img(dir.absoluteFilePath(fileName));
    QSize fontSize = img.size() / 16;

    GLuint tex_id;

    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    QImage imgGL = QGLWidget::convertToGLFormat(img.mirrored());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits());

    glBindTexture(GL_TEXTURE_2D, 0);


    _textures.insert(tag, { tex_id, fontSize } );
  }
}

Fonts::~Fonts() {
  for (auto tex: _textures.values())
    glDeleteTextures(1, &tex.first);
}
