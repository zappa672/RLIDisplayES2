#include "mainwidget.h"

#include "common/properties.h"

#include <QDateTime>
#include <QApplication>

#include <iostream>


using namespace RLI;

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWidget construction start";

  _state.peleng_count = qApp->property(PROPERTY_PELENG_COUNT).toInt();
  _state.peleng_size = qApp->property(PROPERTY_PELENG_SIZE).toInt();

  _data_sources.insert(DataSource::Radar, new RadarDataSource(qApp->property(PROPERTY_DATA_DELAY).toInt(), this));
  _data_sources.insert(DataSource::Ship, new ShipDataSource(1000, this));
  _data_sources.insert(DataSource::Target, new TargetDataSource(1000, this));

  for (auto ds: _data_sources)
    ds->start();

  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWidget construction finish";
}

MainWidget::~MainWidget() {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "~MainWidget start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete datasources";
  for (auto ds: _data_sources) {
    ds->stop();
    delete ds;
  }

  if (_timerId == -1)
    return;

  killTimer(_timerId);

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete fonts";
  delete _fonts;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete simple layers";
  for (auto lr: _simple_layers)
    delete lr;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete fbo layers";
  for (auto lr: _tex_layers)
    delete lr;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete Info layer";
  delete _info_layer;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "~MainWidget finish";
}

void MainWidget::debugInfo() {
  std::cout << "OpenGL debug info start" << std::endl;
  std::cout << "-----------------------" << std::endl;
  std::cout << "Vendor: " << reinterpret_cast<const char*>( glGetString(GL_VENDOR) ) << std::endl;
  std::cout << "Renderer: " << reinterpret_cast<const char*>( glGetString(GL_RENDERER) ) << std::endl;
  std::cout << "OpenGL: " << reinterpret_cast<const char*>( glGetString(GL_VERSION) ) << std::endl;
  std::cout << "Shaders: " << reinterpret_cast<const char*>( glGetString(GL_SHADING_LANGUAGE_VERSION) ) << std::endl;

  std::cout << std::endl << "Extensions: " << reinterpret_cast<const char*>( glGetString(GL_EXTENSIONS) ) << std::endl << std::endl;

  int val;
  glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &val);
  std::cout << "GL_MAX_FRAMEBUFFER_WIDTH: " << val << std::endl;

  glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &val);
  std::cout << "GL_MAX_FRAMEBUFFER_HEIGHT: " << val << std::endl;

  glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &val);
  std::cout << "GL_MAX_FRAMEBUFFER_LAYERS: " << val << std::endl;

  glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &val);
  std::cout << "GL_MAX_FRAMEBUFFER_SAMPLES: " << val << std::endl;

  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
  std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << val << std::endl;

  std::cout << "-----------------------" << std::endl;
}

void MainWidget::timerEvent(QTimerEvent*) {
  update();
}

void MainWidget::initializeGL() {
  QOpenGLWidget::initializeGL();

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
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << "initializeGL" << size() << layout().size;

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
  _tex_layers.insert(TextureLayer::Radar, new RadarEngine(_state, layout(), context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Trail engine init start";
  _tex_layers.insert(TextureLayer::Trail, new RadarEngine(_state, layout(), context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Trail engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init start";
  _tex_layers.insert(TextureLayer::Mask, new MaskEngine(_state, layout(), _fonts, context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";
  MagnifierEngine* magnEngine = new MagnifierEngine(_state , layout(), context(), this);
  magnEngine->setAmplitudesVboId(radarLayer()->ampsVboId());
  magnEngine->setPaletteTextureId(radarLayer()->paletteTexId());
  _tex_layers.insert(TextureLayer::Magnifier, magnEngine);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init start";
  _tex_layers.insert(TextureLayer::Menu, new MenuEngine(layout(), _fonts, context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init start";
  _info_layer = new InfoEngine(layout(), context(), _fonts, this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init start";
  //-------------------------------------------------------------

  connect( radarDS(), SIGNAL(updateRadarData(int, int, GLfloat*))
         , radarLayer(), SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  connect( radarDS(), SIGNAL(updateTrailData(int, int, GLfloat*))
         , trailLayer(), SLOT(updateData(int, int, GLfloat*))
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
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << "resizeGL" << QSize(w, h) << layout().size;

  QOpenGLWidget::resizeGL(w, h);

  _projection.setToIdentity();
  _projection.ortho(geometry());

  if (_timerId == -1)
    return;

  // LayoutManager resize returns false if currentSize was not changed
  if (!_layout_manager.resize(QSize(w, h)))
    return;

  if (_timerId == -1)
    return;

  for (auto lr : _tex_layers)
    lr->resizeTexture(layout());

  _info_layer->resize(layout());
}

void MainWidget::paintGL() {
  if (_timerId == -1)
    return;

  // Update offscreen layers
  // -----------------------------------
  for (auto lr : _tex_layers)
    lr->paint(_state, layout());

  _info_layer->update(_state, layout());
  // -----------------------------------

  // Fill widget
  // -----------------------------------
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, width(), height());

  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  _program.bind();
  glBindVertexArray(_vao_id);

  glActiveTexture(GL_TEXTURE0);

  _program.setUniformValue("texture", 0);
  _program.setUniformValue("mvp_matrix", _projection);

  drawRect(radarLayer()->geometry(), radarLayer()->texId());
  drawRect(trailLayer()->geometry(), trailLayer()->texId());

  drawRect(maskLayer()->geometry(), maskLayer()->texId());

  drawRect(menuLayer()->geometry(), menuLayer()->texId());

  drawRect(magnifierLayer()->geometry(), magnifierLayer()->texId());

  for (auto block: _info_layer->blocks())
    drawRect(block.second->geometry(), block.second->texId());

  glBindVertexArray(0);
  _program.release();
  // -----------------------------------

  glFlush();
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
