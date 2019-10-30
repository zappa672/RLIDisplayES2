#include "mainwidget.h"

#include "common/properties.h"

#include <QDateTime>
#include <QApplication>

using namespace RLI;

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWidget construction start";

  _state.peleng_count = qApp->property(PROPERTY_PELENG_COUNT).toInt();
  _state.peleng_size = qApp->property(PROPERTY_PELENG_SIZE).toInt();

  _ds_radar = new RadarDataSource(this);
  _ds_radar->start();

  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWidget construction finish";
}

MainWidget::~MainWidget() {
  _ds_radar->stop();
  delete _ds_radar;

  if (_timerId == -1)
    return;

  killTimer(_timerId);

  delete _fonts;

  delete _lr_radar;
  delete _lr_trail;
  delete _lr_mask;
  delete _lr_magn;
}

void MainWidget::debugInfo() {
  qDebug() << "";
  qDebug() << "Vendor: " << reinterpret_cast<const char*>( glGetString(GL_VENDOR) );
  qDebug() << "Renderer: " << reinterpret_cast<const char*>( glGetString(GL_RENDERER) );
  qDebug() << "OpenGL: " << reinterpret_cast<const char*>( glGetString(GL_VERSION) );
  qDebug() << "Shaders: " << reinterpret_cast<const char*>( glGetString(GL_SHADING_LANGUAGE_VERSION) );

//  qDebug() << "";
//  qDebug() << "Extensions: ";
//  QString exts((const char*) glGetString(GL_EXTENSIONS));
//  for (QString ext : exts.split(" "))
//     qDebug() << "\t" << ext;
//  qDebug() << "";

//  int val;
//  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
//  qDebug() << "Max texture image units: " << val;
//  qDebug() << "";
}

void MainWidget::timerEvent(QTimerEvent*) {
  update();
}

void MainWidget::initializeGL() {
  if (_timerId != -1)
    return;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init start";

  initializeOpenGLFunctions();

  debugInfo();

  glDisable(GL_STENCIL);
  glDisable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  _layout_manager.resize(size());

  _projection.setToIdentity();
  _projection.ortho(geometry());

  initProgram();
  initBuffers();

  // Layers initialization
  //-------------------------------------------------------------

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Fonts init start";
  _fonts = new Fonts(context(), "assets/textures/fonts");
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Fonts init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init start";
  _lr_radar = new RadarEngine(_state, layout(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Trail engine init start";
  _lr_trail = new RadarEngine(_state, layout(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Trail engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init start";
  _lr_mask = new MaskEngine(_state, layout(), _fonts, context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";
  _lr_magn = new MagnifierEngine(_state, layout(), _lr_radar->ampsVboId(), _lr_radar->paletteTexId(), context(), this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";

  //-------------------------------------------------------------

  connect( _ds_radar, SIGNAL(updateRadarData(int, int, GLfloat*))
         , _lr_radar, SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  connect( _ds_radar, SIGNAL(updateTrailData(int, int, GLfloat*))
         , _lr_trail, SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  //-------------------------------------------------------------

  _timerId = startTimer(33, Qt::CoarseTimer);

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "GL init finish";
}

void MainWidget::initBuffers() {
  glGenVertexArrays(1, &_vao_id);
  glGenBuffers(ATTR_COUNT, _vbo_ids);

  static const GLfloat texcoords[] = {  0.0f,  1.0f
                                     ,  0.0f,  0.0f
                                     ,  1.0f,  1.0f
                                     ,  1.0f,  0.0f  };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_TEXCOORD]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_TEXCOORD]);
  glVertexAttribPointer(_attr_locs[ATTR_TEXCOORD], 2,  GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_TEXCOORD]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glVertexAttribPointer(_attr_locs[ATTR_POSITION], 2,  GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(_attr_locs[ATTR_POSITION]);

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
  _lr_mask->resizeTexture(layout());
  _lr_magn->resizeTexture(layout());
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
  _lr_mask->paint(_state, layout());
  _lr_magn->paint(_state, layout());
}

void MainWidget::paintLayers() {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, width(), height());

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  QMatrix4x4 transform;
  transform.setToIdentity();

  _program.bind();

  glActiveTexture(GL_TEXTURE0);

  _program.setUniformValue("texture", 0);
  _program.setUniformValue("mvp_matrix", _projection*transform);

  glBindVertexArray(_vao_id);

  drawRect(_lr_radar->rect(), _lr_radar->texId());
  drawRect(_lr_trail->rect(), _lr_trail->texId());

  drawRect(_lr_mask->rect(), _lr_mask->texId());

  drawRect(_lr_magn->rect(), _lr_magn->texId());

  glBindVertexArray(0);

  _program.release();
}


void MainWidget::drawRect(const QRect& rect, GLuint textureId) {
  GLfloat vertices[] = { static_cast<GLfloat>(rect.left()),  static_cast<GLfloat>(rect.bottom())
                       , static_cast<GLfloat>(rect.left()),  static_cast<GLfloat>(rect.top())
                       , static_cast<GLfloat>(rect.right()), static_cast<GLfloat>(rect.bottom())
                       , static_cast<GLfloat>(rect.right()), static_cast<GLfloat>(rect.top()) };

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindTexture(GL_TEXTURE_2D, 0);
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
