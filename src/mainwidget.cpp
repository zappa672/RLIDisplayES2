#include "mainwidget.h"

using namespace RLI;

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent) {

  _ds_radar = new RadarDataSource(this);
  _ds_radar->start();

}

MainWidget::~MainWidget() {
  _ds_radar->stop();
  delete _ds_radar;

  if (_timerId == -1)
    return;

  killTimer(_timerId);
  delete _layer;
  delete _program;
}


void MainWidget::timerEvent(QTimerEvent*) {
  _layer->update();

  update();
}

void MainWidget::initializeGL() {
  initializeOpenGLFunctions();

  glDisable(GL_STENCIL);
  glDisable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  _program = new QOpenGLShaderProgram(this);
  initProgram();

  _layer = new FboLayerBase(QSize(256, 256), context(), this);

  _timerId = startTimer(33, Qt::CoarseTimer);
}

void MainWidget::resizeGL(int w, int h) {
  _layout_manager.resize(QSize(w, h));

  _projection.setToIdentity();
  _projection.ortho(geometry());

  if (_timerId == -1)
    return;

  Layout* layout = _layout_manager.layout();

  _layer->resize(layout->circle.box_rect.size());
}

void MainWidget::paintGL() {
  if (_timerId == -1)
    return;

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  glViewport(0, 0, width(), height());

  glClearColor(0.3f, 0.5f, 0.4f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  Layout* layout = _layout_manager.layout();

  drawRect(layout->circle.box_rect, _layer->textureId());

  glFlush();
}

void MainWidget::initProgram() {
  _program->addShaderFromSourceCode(QOpenGLShader::Vertex,
"uniform mat4 mvp_matrix; \
attribute vec2 position; \
attribute vec2 texcoord; \
varying vec2 v_texcoord; \
 \
void main() { \
  gl_Position = mvp_matrix * vec4(position, 0.0, 1.0); \
  v_texcoord = texcoord; \
}");

  _program->addShaderFromSourceCode(QOpenGLShader::Fragment,
"uniform sampler2D texture; \
varying vec2 v_texcoord; \
 \
void main() { \
  gl_FragColor = texture2D(texture, v_texcoord); \
}");

  _program->link();
  _program->bind();

  GLuint id = _program->programId();

  _unif_locs[UNIF_TEXTURE]    = glGetUniformLocation(id, "texture");
  _unif_locs[UNIF_MVPMATRIX]  = glGetUniformLocation(id, "mvp_matrix");

  _attr_locs[ATTR_POSITION]   = static_cast<GLuint>(glGetAttribLocation(id, "position"));
  _attr_locs[ATTR_TEXCOORD]   = static_cast<GLuint>(glGetAttribLocation(id, "texcoord"));

  _program->release();
}

void MainWidget::drawRect(const QRectF& rect, GLuint textureId) {
  static const GLfloat vertices[] =  { static_cast<float>(rect.left()),  static_cast<float>(rect.bottom())
                                     , static_cast<float>(rect.left()),  static_cast<float>(rect.top())
                                     , static_cast<float>(rect.right()), static_cast<float>(rect.bottom())
                                     , static_cast<float>(rect.right()), static_cast<float>(rect.top()) };

  static const GLfloat texcoords[] = { 0.0f, 1.0f
                                     , 0.0f, 0.0f
                                     , 1.0f, 1.0f
                                     , 1.0f, 0.0f };

  _program->bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);

  QMatrix4x4 transform;
  transform.setToIdentity();
  transform.translate( 0.f, 0.f, 0.f );

  glUniform1f(_unif_locs[UNIF_TEXTURE], 0);
  _program->setUniformValue(_unif_locs[UNIF_MVPMATRIX], _projection*transform);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[ATTR_POSITION], 2,  GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_TEXCOORD]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
  glVertexAttribPointer( _attr_locs[ATTR_TEXCOORD], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_TEXCOORD]);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _program->release();
}
