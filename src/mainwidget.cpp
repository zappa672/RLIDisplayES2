#include "mainwidget.h"

#include "common/properties.h"

#include <QScreen>
#include <QDateTime>
#include <QApplication>
#include <QGuiApplication>

#include <iostream>
#include <cmath>
#include <qmath.h>


using namespace RLI;

MainWidget::MainWidget(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWidget construction start";

  _state.peleng_count = qApp->property(PROPERTY_PELENG_COUNT).toInt();
  _state.peleng_size = qApp->property(PROPERTY_PELENG_SIZE).toInt();

  _data_sources.emplace(DataSource::Radar, new RadarDataSource(qApp->property(PROPERTY_DATA_DELAY).toInt(), this));
  _data_sources.emplace(DataSource::Ship, new ShipDataSource(1000, this));
  _data_sources.emplace(DataSource::Target, new TargetDataSource(1000, this));

  for (auto ds: _data_sources)
    ds.second->start();

  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "MainWidget construction finish";
}

MainWidget::~MainWidget() {
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "~MainWidget start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete datasources";
  for (auto ds: _data_sources) {
    ds.second->stop();
    delete ds.second;
  }

  if (_timerId == -1)
    return;

  killTimer(_timerId);

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete fonts";
  delete _fonts;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete simple layers";
  for (auto lr: _simple_layers)
    delete lr.second;

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Delete fbo layers";
  for (auto lr: _tex_layers)
    delete lr.second;

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
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
  std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << val << std::endl;

  //glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &val);
  //std::cout << "GL_MAX_FRAMEBUFFER_WIDTH: " << val << std::endl;
  //
  //glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &val);
  //std::cout << "GL_MAX_FRAMEBUFFER_HEIGHT: " << val << std::endl;
  //
  //glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &val);
  //std::cout << "GL_MAX_FRAMEBUFFER_LAYERS: " << val << std::endl;
  //
  //glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &val);
  //std::cout << "GL_MAX_FRAMEBUFFER_SAMPLES: " << val << std::endl;

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

  _state.projection.setToIdentity();
  _state.projection.ortho(QRect(QPoint(0, 0), size()));

  initProgram();
  initBuffers();

  // Layers initialization
  //-------------------------------------------------------------
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Fonts init start";
  _fonts = new Fonts(context(), "assets/textures/fonts");
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Fonts init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init start";
  _tex_layers.emplace(TextureLayer::Radar, new RadarEngine(_state, layout(), context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Radar engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Trail engine init start";
  _tex_layers.emplace(TextureLayer::Trail, new RadarEngine(_state, layout(), context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Trail engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init start";
  _tex_layers.emplace(TextureLayer::Mask, new MaskEngine(_state, layout(), _fonts, context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Mask engine init finish";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";
  MagnifierEngine* magnEngine = new MagnifierEngine(_state , layout(), context(), this);
  magnEngine->setAmplitudesVboId(radarLayer()->ampsVboId());
  magnEngine->setPaletteTextureId(radarLayer()->paletteTexId());
  _tex_layers.emplace(TextureLayer::Magnifier, magnEngine);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Magnifier engine init start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init start";
  _tex_layers.emplace(TextureLayer::Menu, new MenuEngine(layout(), _fonts, context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Menu engine init start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init start";
  _info_layer = new InfoEngine(layout(), context(), _fonts, this);
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Info engine init start";


  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Controls engine init start";
  _simple_layers.emplace(SimpleLayer::Controls, new ControlsEngine(context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Controls engine init start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Route engine init start";
  _simple_layers.emplace(SimpleLayer::Route, new RouteEngine(context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Route engine init start";

  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Targets engine init start";
  _simple_layers.emplace(SimpleLayer::Targets, new TargetEngine(context(), this));
  qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss zzz") << ": " << "Targets engine init start";
  //-------------------------------------------------------------

  connect( radarDS(), SIGNAL(updateRadarData(int, int, GLfloat*))
         , radarLayer(), SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  connect( radarDS(), SIGNAL(updateTrailData(int, int, GLfloat*))
         , trailLayer(), SLOT(updateData(int, int, GLfloat*))
         , Qt::QueuedConnection );

  //-------------------------------------------------------------

  _timerId = startTimer(qApp->property(PROPERTY_FRAME_DELAY).toInt(), Qt::CoarseTimer);

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

  _state.projection.setToIdentity();
  _state.projection.ortho(QRect(QPoint(0, 0), size()));

  if (_timerId == -1)
    return;

  // LayoutManager resize returns false if currentSize was not changed
  if (!_layout_manager.resize(QSize(w, h)))
    return;

  if (_timerId == -1)
    return;

  for (auto lr : _tex_layers)
    lr.second->resizeTexture(layout());

  infoLayer()->resize(layout());


  infoLayer()->secondChanged();
  infoLayer()->setFps(static_cast<int>(frameRate()));
  infoLayer()->onPositionChanged(_state.ship_position);
  infoLayer()->onTargetCountChanged(targetLayer()->targetCount());
  infoLayer()->onSelectedTargetUpdated(targetLayer()->selectedTag(), targetLayer()->selectedTrgt());

  infoLayer()->onScaleChanged(&_state.radar_scale);
  _state.chart_scale = (_state.radar_scale.current()->len * Math::MILE2METER) / layout().circle.radius;

  infoLayer()->updateGain(_state.gain);
  infoLayer()->updateWater(_state.water);
  infoLayer()->updateRain(_state.rain);
  infoLayer()->updateApch(_state.apch);
  infoLayer()->updateEmission(_state.emission);
}


float MainWidget::frameRate() {
  if (_frameTimes.size() < 2)
    return 0.f;

  QDateTime f = _frameTimes.first();
  QDateTime l = _frameTimes.last();
  int count = _frameTimes.size() - 1;

  return 1000.f / (f.msecsTo(l) / count);
}



void MainWidget::paintGL() {
  if (_timerId == -1)
    return;

  QDateTime time = QDateTime::currentDateTime();

  if (_frameTimes.size() == 0 || _frameTimes.last().time().second() != time.time().second())
    _info_layer->secondChanged();

  _frameTimes.push_back(time);
  while (_frameTimes.size() > 20)
    _frameTimes.removeFirst();

  _info_layer->setFps(static_cast<int>(frameRate()));


  // Update offscreen layers
  // -----------------------------------
  for (auto lr : _tex_layers)
    lr.second->paint(_state, layout());

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
  _program.setUniformValue("mvp_matrix", _state.projection);

  drawRect(radarLayer()->geometry(), radarLayer()->texId());
  drawRect(trailLayer()->geometry(), trailLayer()->texId());

  _program.release();
  glBindVertexArray(0);

  targetLayer()->paint(_state, layout());
  routeLayer()->paint(_state, layout());
  cntrlLayer()->paint(_state, layout());

  _program.bind();
  glBindVertexArray(_vao_id);

  drawRect(maskLayer()->geometry(), maskLayer()->texId());

  drawRect(menuLayer()->geometry(), menuLayer()->texId());

  if (_state.state == WidgetState::MAGNIFIER)
    drawRect(magnifierLayer()->geometry(), magnifierLayer()->texId());

  for (auto block: infoLayer()->blocks())
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


void MainWidget::keyReleaseEvent(QKeyEvent *event) {
  _pressed_keys.remove(event->key());
  QOpenGLWidget::keyReleaseEvent(event);
}


void MainWidget::keyPressEvent(QKeyEvent* event) {
  _pressed_keys.insert(event->key());
  auto mod_keys = event->modifiers();

  if ( std::find(MENU_ENABLED_STATES.cbegin(), MENU_ENABLED_STATES.cend(), _state.state) != MENU_ENABLED_STATES.cend() )
    menuLayer()->onKeyPressed(event);

  switch(event->key()) {
  case Qt::Key_PageUp:
    if (mod_keys & Qt::ControlModifier)
      emit infoLayer()->updateGain( _state.gain = qMin(_state.gain + 5.0f, 255.0f) );

    if (mod_keys & Qt::AltModifier)
      emit infoLayer()->updateWater( _state.water = qMin(_state.water + 5.0f, 255.0f) );

    if (mod_keys & Qt::ShiftModifier)
      emit infoLayer()->updateRain(  _state.rain = qMin(_state.rain + 5.0f, 255.0f) );

    break;

  case Qt::Key_PageDown:
    if (mod_keys & Qt::ControlModifier)
      emit infoLayer()->updateGain( _state.gain = qMax(_state.gain - 5.0f, 0.0f) );

    if (mod_keys & Qt::AltModifier)
      emit infoLayer()->updateWater( _state.water = qMax(_state.water - 5.0f, 0.0f) );

    if (mod_keys & Qt::ShiftModifier)
      emit infoLayer()->updateRain( _state.rain = qMax(_state.rain - 5.0f, 0.0f) );

    break;

  // Под. имп. Помех
  case Qt::Key_S:
    break;

  // Меню
  case Qt::Key_W:
    if( _pressed_keys.contains(Qt::Key_B) ) {
      if (_state.state == WidgetState::CONFIG_MENU)
        _state.state = WidgetState::DEFAULT;
      else if (_state.state == WidgetState::DEFAULT)
        _state.state = WidgetState::CONFIG_MENU;

    } else {
      if (_state.state == WidgetState::MAIN_MENU)
        _state.state = WidgetState::DEFAULT;
      else if (_state.state == WidgetState::DEFAULT)
        _state.state = WidgetState::MAIN_MENU;
    }

    menuLayer()->onStateChanged(_state.state);
    break;

  // Шкала +
  case Qt::Key_Plus:
    if (_state.state == WidgetState::ROUTE_EDITION) {
      QPointF pos = QPointF( sin(Math::rads(_state.vn_p)) * _state.vd
                           ,-cos(Math::rads(_state.vn_p)) * _state.vd );
      //float scale = (_rli_scale.len*RLIMath::MILE2METER) / _maskEngine->getRadius();
      GeoPos last_route_point = routeLayer()->getLastPoint();
      GeoPos cursor_coords = Math::pos_to_coords( last_route_point, QPoint(0, 0), pos, _state.chart_scale);
      _state.visir_center_pos = cursor_coords;
      routeLayer()->addPointToCurrent(cursor_coords);

      break;
    }

    _state.radar_scale.prevScale();
    infoLayer()->onScaleChanged(&_state.radar_scale);
    _state.chart_scale = (_state.radar_scale.current()->len * Math::MILE2METER) / _layout_manager.layout().circle.radius;
    break;

  // Шкала -
  case Qt::Key_Minus:
    if (_state.state == WidgetState::ROUTE_EDITION) {
      routeLayer()->removePointFromCurrent();
      _state.visir_center_pos = routeLayer()->getLastPoint();
      break;
    }

    _state.radar_scale.nextScale();
    infoLayer()->onScaleChanged(&_state.radar_scale);
    _state.chart_scale = (_state.radar_scale.current()->len * Math::MILE2METER) / _layout_manager.layout().circle.radius;
    break;

  // Вынос центра
  case Qt::Key_C:
    _state.center_shift = _state.cursor_pos;
    break;

  // Скрытое меню
  case Qt::Key_U:
    break;

  // Следы точки
  case Qt::Key_T:
    break;

  // Выбор цели
  case Qt::Key_Up:
    if (mod_keys & Qt::ControlModifier) {
      //_state.ship_position.lat += 0.010;
      if (_state.magn_min_rad + _state.magn_height < 800) {
        _state.magn_min_rad += 1;
      }
    } else {
      _state.vd += 1.0;
      infoLayer()->onVdChanged(_state);
    }
    break;

  // ЛИД / ЛОД
  case Qt::Key_Down:
    if (mod_keys & Qt::ControlModifier) {
      //_state.ship_position.lat -= 0.010;
      if (_state.magn_min_rad > 0) {
        _state.magn_min_rad -= 1;
      }
    } else {
      _state.vd = qMax(0.0, _state.vd - 1.0);
      infoLayer()->onVdChanged(_state);
    }
    break;

  case Qt::Key_Left:
    if (mod_keys & Qt::ControlModifier) {
      //_state.ship_position.lon -= 0.005;
      _state.magn_min_peleng = (4096 + _state.magn_min_peleng - 1) % 4096;
    } else if (mod_keys & Qt::AltModifier) {
      _state.vn_cu = fmod(_state.vn_cu - 1.0, 360.0);
    } else if (mod_keys & Qt::ShiftModifier) {
      _state.course_mark_angle = fmod(_state.course_mark_angle - 1.0, 360.0);
    } else {
      _state.vn_p = fmod(_state.vn_p - 1.0, 360.0);
      infoLayer()->onVnChanged(_state);
    }
    break;

  case Qt::Key_Right:
    if (mod_keys & Qt::ControlModifier) {
      //_state.ship_position.lon += 0.005;
      _state.magn_min_peleng = (4096 + _state.magn_min_peleng + 1) % 4096;
    } else if (mod_keys & Qt::AltModifier) {
      _state.vn_cu = fmod(_state.vn_cu + 1.0, 360.0);
    } else if (mod_keys & Qt::ShiftModifier) {
      _state.course_mark_angle = fmod(_state.course_mark_angle + 1.0, 360.0);
    } else {
      _state.vn_p = fmod(_state.vn_p + 1.0, 360.0);
      infoLayer()->onVnChanged(_state);
    }
    break;

  // Захват
  case Qt::Key_Return:
  case Qt::Key_Enter:
    break;

  //Сброс
  case Qt::Key_Escape:
    break;

  // Парал. Линии
  case Qt::Key_Backslash:
    _state.show_parallel = !_state.show_parallel;
    break;

  //Электронная лупа
  case Qt::Key_L:
    if ( _state.state == WidgetState::DEFAULT )
      _state.state = WidgetState::MAGNIFIER;
    else if ( _state.state == WidgetState::MAGNIFIER )
      _state.state = WidgetState::DEFAULT;
    break;

  //Обзор
  case Qt::Key_X:
    if (width() == 1024) {
      setWindowState(Qt::WindowFullScreen);
    } else {
      setGeometry(QRect(QPoint(0, 0), QSize(1024, 768)));
    }
    break;

  //Узкий / Шир.
  case Qt::Key_Greater:
    break;

  //Накоп. Видео
  case Qt::Key_V:
    break;

  //Сброс АС
  case Qt::Key_Q:
    break;

  //Манёвр
  case Qt::Key_M:
    break;

  //Курс / Север / Курс стаб
  case Qt::Key_H:
    switch(_state.orientation) {
      case Orientation::HEAD:
        _state.orientation = Orientation::NORTH;
        _state.mode = Mode::X;
        break;
      case Orientation::NORTH:
        _state.orientation = Orientation::COURSE;
        _state.mode = Mode::S;
        break;
      case Orientation::COURSE:
        _state.orientation = Orientation::HEAD;
        _state.mode = Mode::T;
        break;
    }
    infoLayer()->onOrientationChanged(_state.orientation);
    break;

  //ИД / ОД
  case Qt::Key_R:
    break;

  //НКД
  case Qt::Key_D:
    break;

  //Карта (Маршрут)
  case Qt::Key_A:
    break;

  //Выбор
  case Qt::Key_G:
    break;

  //Стоп-кадр
  case Qt::Key_F:
    break;

  //Откл. Звука
  case Qt::Key_B:
    break;

  //Откл. ОК
  case Qt::Key_K:
    break;

  //Вынос ВН/ВД
  case Qt::Key_Slash:
    _state.show_circles = !_state.show_circles;
    break;
  }

  QOpenGLWidget::keyPressEvent(event);
}

void MainWidget::mouseMoveEvent(QMouseEvent* event) {
  auto diff = event->pos() - _layout_manager.layout().circle.center;
  QVector2D diffV(diff);
  if (diffV.length() < 0.66f * _layout_manager.layout().circle.radius)
    _state.cursor_pos = diff;

  double bearing = Math::degs(qAtan2(static_cast<double>(diffV.x()), static_cast<double>(-diffV.y())));

  if (bearing < 0 && _state.orientation == Orientation::NORTH)
    bearing = 360 + bearing;

  infoLayer()->onCursorPosChanged( static_cast<double>(diffV.length()), bearing );
}

void MainWidget::mousePressEvent(QMouseEvent* event) {
  auto coords = Math::pos_to_coords( _state.ship_position
                                   , _layout_manager.layout().circle.center
                                   , event->pos()
                                   , _state.chart_scale );
  targetLayer()->select(coords, _state.chart_scale);
}
