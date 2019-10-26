#include "mainwidget.h"

#include "common/properties.h"

#include <QApplication>

using namespace RLI;

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent) {
  _state.peleng_count = qApp->property(PROPERTY_PELENG_COUNT).toInt();
  _state.peleng_size = qApp->property(PROPERTY_PELENG_SIZE).toInt();

  _ds_radar = new RadarDataSource(this);
  _ds_radar->start();
}

MainWidget::~MainWidget() {
  _ds_radar->stop();
  delete _ds_radar;

  if (_timerId == -1)
    return;

  killTimer(_timerId);

  delete _lr_radar;
  delete _lr_trail;
}


void MainWidget::timerEvent(QTimerEvent*) {
  update();
}

void MainWidget::initializeGL() {
  if (_timerId != -1)
    return;

  initializeOpenGLFunctions();

  glDisable(GL_STENCIL);
  glDisable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  _layout_manager.resize(size());

  _projection.setToIdentity();
  _projection.ortho(geometry());

  initProgram();
  initBuffers();

  _lr_radar = new RadarEngine(_state, layout(), context(), this);

  _lr_trail = new RadarEngine(_state, layout(), context(), this);


  connect( _ds_radar, SIGNAL(updateRadarData(int, int, GLfloat*))
         , _lr_radar, SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  connect( _ds_radar, SIGNAL(updateTrailData(int, int, GLfloat*))
         , _lr_trail, SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  _timerId = startTimer(33, Qt::CoarseTimer);
}

void MainWidget::initBuffers() {
  glGenVertexArrays(1, &_vao_id);
  glGenBuffers(ATTR_COUNT, _vbo_ids);

  static const GLfloat vertices[] =  { -1.0f,  1.0f
                                     , -1.0f, -1.0f
                                     ,  1.0f,  1.0f
                                     ,  1.0f, -1.0f  };

  static const GLfloat texcoords[] = {  0.0f,  1.0f
                                     ,  0.0f,  0.0f
                                     ,  1.0f,  1.0f
                                     ,  1.0f,  0.0f  };

  glBindVertexArray(_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[ATTR_POSITION], 2,  GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_TEXCOORD]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[ATTR_TEXCOORD], 2,  GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_TEXCOORD]);

  glBindVertexArray(0);
}

void MainWidget::resizeGL(int w, int h) {
  if (_timerId == -1)
    return;

  // LayoutManager resize returns false if currentSize was not changed
  if (!_layout_manager.resize(QSize(w, h)))
    return;

  _projection.setToIdentity();
  _projection.ortho(geometry());

  if (_timerId == -1)
    return;

  _lr_radar->resizeTexture(layout());
  _lr_trail->resizeTexture(layout());
}

void MainWidget::paintGL() {
  if (_timerId == -1)
    return;

  updateLayers();
  paintLayers();

  glFlush();
}

void MainWidget::updateLayers() {
  _lr_radar->paint(_state, layout());
  _lr_trail->paint(_state, layout());
}

void MainWidget::paintLayers() {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, width(), height());

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);


  drawRect(_lr_radar->rect(), _lr_radar->texId());
  drawRect(_lr_trail->rect(), _lr_trail->texId());
}

void MainWidget::initProgram() {
  _program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/main.vert.glsl");
  _program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/main.frag.glsl");

  _program.link();
  _program.bind();

  _attr_locs[ATTR_POSITION] = static_cast<GLuint>(_program.attributeLocation( "position"));
  _attr_locs[ATTR_TEXCOORD] = static_cast<GLuint>(_program.attributeLocation("texcoord"));

  _program.release();
}

void MainWidget::drawRect(const QRectF& rect, GLuint textureId) {
  static const GLfloat vertices[] =  { static_cast<float>(rect.left()),  static_cast<float>(rect.bottom())
                                     , static_cast<float>(rect.left()),  static_cast<float>(rect.top())
                                     , static_cast<float>(rect.right()), static_cast<float>(rect.bottom())
                                     , static_cast<float>(rect.right()), static_cast<float>(rect.top()) };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _program.bind();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);

  QMatrix4x4 transform;
  transform.setToIdentity();

  _program.setUniformValue("texture", 0);
  _program.setUniformValue("mvp_matrix", _projection*transform);

  glBindVertexArray(_vao_id);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray(0);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _program.release();
}
