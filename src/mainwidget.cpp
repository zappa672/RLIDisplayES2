#include "mainwidget.h"

#include "common/properties.h"

#include <QApplication>

using namespace RLI;

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent) {

  _state.peleng_count = qApp->property(PROPERTY_BEARINGS_PER_CYCLE).toInt();
  _state.peleng_length = qApp->property(PROPERTY_PELENG_SIZE).toInt();

  _ds_radar = new RadarDataSource(this);
  _ds_radar->start();

}

MainWidget::~MainWidget() {
  _ds_radar->stop();
  delete _ds_radar;

  if (_timerId == -1)
    return;

  killTimer(_timerId);
  delete _radarLayer;
  delete _program;
}


void MainWidget::timerEvent(QTimerEvent*) {
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

  _radarLayer = new RadarEngine(_state, _layout_manager.layout(), context(), this);

  connect( _ds_radar, SIGNAL(updateRadarData(int, int, GLfloat*))
         , _radarLayer, SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  _timerId = startTimer(33, Qt::CoarseTimer);
}

void MainWidget::resizeGL(int w, int h) {
  _layout_manager.resize(QSize(w, h));

  _projection.setToIdentity();
  _projection.ortho(geometry());

  if (_timerId == -1)
    return;

  Layout* layout = _layout_manager.layout();

  _radarLayer->resizeTexture(layout);
}

void MainWidget::paintGL() {
  if (_timerId == -1)
    return;

  updateLayers();
  paintLayers();
  glFlush();
}

void MainWidget::updateLayers() {
  _radarLayer->updateTexture(_state);
}

void MainWidget::paintLayers() {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, width(), height());

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  Layout* layout = _layout_manager.layout();

  drawRect(layout->circle.box_rect, _radarLayer->textureId());
}

void MainWidget::initProgram() {
  _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/main.vert.glsl");
  _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/main.frag.glsl");

  _program->link();
  _program->bind();

  GLuint id = _program->programId();

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

  _program->setUniformValue("texture", 0);
  _program->setUniformValue("mvp_matrix", _projection*transform);

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
