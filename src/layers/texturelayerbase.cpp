#include "texturelayerbase.h"

using namespace RLI;

TextureLayerBase::TextureLayerBase(const QRect& rect, QOpenGLContext* context, QObject* parent) : LayerBase(context, parent) {
  resize(rect);
}

TextureLayerBase::~TextureLayerBase() {
  if (!_fbo_rect.isEmpty()) {
    glDeleteFramebuffers(1, &_fbo_id);
    glDeleteRenderbuffers(1, &_depth_rbo_id);
    glDeleteTextures(1, &_fbo_tex_id);
  }
}

void TextureLayerBase::clear(float r, float g, float b, float a, float d) {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);

  glClearDepthf(d);
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureLayerBase::resize(const QRect& rect) {
  if (rect.width() <= 0 || rect.width() <= 0 || rect == _fbo_rect)
    return;

  if (!_fbo_rect.isEmpty()) {
    glDeleteFramebuffers(1, &_fbo_id);
    glDeleteRenderbuffers(1, &_depth_rbo_id);
    glDeleteTextures(1, &_fbo_tex_id);
  }

  glGenFramebuffers(1, &_fbo_id);
  glGenRenderbuffers(1, &_depth_rbo_id);
  glGenTextures(1, &_fbo_tex_id);

  glBindTexture(GL_TEXTURE_2D, _fbo_tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rect.width(), rect.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, _depth_rbo_id);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, rect.width(), rect.height());
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo_id);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbo_tex_id, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_rbo_id);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _fbo_rect = rect;

  _projection.setToIdentity();
  _projection.ortho(0.f, rect.width(), 0.f, rect.height(), -1.f, 1.f);

  clear(1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
}
