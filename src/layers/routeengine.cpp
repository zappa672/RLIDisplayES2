#include "routeengine.h"

using namespace RLI;

RouteEngine::RouteEngine(QOpenGLContext* context, QObject* parent) : LayerBase(context, parent) {
  _currentRoute.push_back(QVector2D(15.3730f, 145.6600f));
  _currentRoute.push_back(QVector2D(15.3640f, 145.7200f));
  _currentRoute.push_back(QVector2D(15.3340f, 145.8000f));
  _currentRoute.push_back(QVector2D(15.3040f, 145.8300f));
  _currentRoute.push_back(QVector2D(15.5440f, 145.9600f));

  saveTo(1);

  initializeOpenGLFunctions();

  glGenBuffers(ATTR_COUNT, _vbo_ids[0]);
  glGenBuffers(ATTR_COUNT, _vbo_ids[1]);
  glGenBuffers(ATTR_COUNT, _vbo_ids[2]);

  static const QMap<QString, int> attr_inices
  { { "world_coords", ATTR_COORDS } };

  static const QMap<QString, int> unif_inices
  { { "mvp_matrix", UNIF_MVP_MATRIX }
  , { "center",     UNIF_CENTER     }
  , { "scale",      UNIF_SCALE      } };

  initShaderProgram(":/shaders/route", attr_inices, unif_inices);

  loadBuffers();
}

RouteEngine::~RouteEngine() {
  glDeleteBuffers(ATTR_COUNT, _vbo_ids[0]);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids[1]);
  glDeleteBuffers(ATTR_COUNT, _vbo_ids[2]);
}

void RouteEngine::clearCurrentRoute() {
  _routesMutex.lock();
  _currentRoute.clear();
  _need_reload_buffer = true;
  _routesMutex.unlock();
}

void RouteEngine::addPointToCurrent(const GeoPos& p) {
  _routesMutex.lock();
  _currentRoute.push_back(p);
  _need_reload_buffer = true;
  _routesMutex.unlock();
}

void RouteEngine::removePointFromCurrent() {
  _routesMutex.lock();
  if (_currentRoute.size() > 1)
    _currentRoute.removeLast();
  _need_reload_buffer = true;
  _routesMutex.unlock();
}

void RouteEngine::loadFrom(int index) {
  _routesMutex.lock();
  if (index >= 0 && index < ROUTES_COUNT)
    _currentRoute = _routes[index];
  _need_reload_buffer = true;
  _routesMutex.unlock();
}

void RouteEngine::saveTo(int index) {
  _routesMutex.lock();
  if (index >= 0 && index < ROUTES_COUNT)
    _routes[index] = _currentRoute;
  //_need_reload_buffer = true;
  _routesMutex.unlock();
}




void RouteEngine::paint(const State& state, const Layout& layout) {
  _routesMutex.lock();

  if (_need_reload_buffer)
    loadBuffers();

  glUseProgram(progId());

  const GeoPos& pos = state.ship_position;

  glUniform1f(unifLoc(UNIF_SCALE), state.chart_scale);
  glUniform2f(unifLoc(UNIF_CENTER), pos.lat, pos.lon);

  glUniformMatrix4fv(unifLoc(UNIF_MVP_MATRIX), 1, GL_FALSE, state.projection.data());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[0][ATTR_COORDS]);
  glVertexAttribPointer(attrLoc(ATTR_COORDS), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_COORDS));

  int pCount = _currentRoute.size();

#if !(defined(GL_ES_VERSION_2_0) || defined(GL_ES_VERSION_3_0))
  glPointSize(5.0);
#endif
  glDrawArrays(GL_POINTS, 0, pCount);

  glLineWidth(2.0);
  glDrawArrays(GL_LINE_STRIP, 0, pCount);


  glLineWidth(1.0);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[1][ATTR_COORDS]);
  glVertexAttribPointer(attrLoc(ATTR_COORDS), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_COORDS));
  glDrawArrays(GL_LINE_STRIP, 0, pCount);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[2][ATTR_COORDS]);
  glVertexAttribPointer(attrLoc(ATTR_COORDS), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_COORDS));
  glDrawArrays(GL_LINE_STRIP, 0, pCount);

  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _routesMutex.unlock();
}


int RouteEngine::loadBuffers() {
  std::vector<GLfloat> ps1, ps2, ps3;

  for (int i = 0; i < _currentRoute.size(); i++) {
    QVector2D p = _currentRoute[i].toVec2D();

    ps1.push_back( p.x() );
    ps1.push_back( p.y() );

    QVector2D norm1 { 0.f, 0.f };
    QVector2D norm2 { 0.f, 0.f };
    QVector2D norm  { 0.f, 0.f };

    if (i > 0) {
      QVector2D tan = p - _currentRoute[i-1].toVec2D();
      tan.normalize();
      norm1 = 0.02*QVector2D(-tan.y(), tan.x());
    }

    if (i < _currentRoute.size()-1) {
      QVector2D tan = _currentRoute[i+1].toVec2D() - p;
      tan.normalize();
      norm2 = 0.02*QVector2D(-tan.y(), tan.x());
    }

    if (_currentRoute.size() > 1) {
      if (i == 0) {
        norm = norm2;
      } else if (i == _currentRoute.size()-1) {
        norm = norm1;
      } else {
        QVector2D p11 = _currentRoute[i-1].toVec2D() + norm1;
        QVector2D p12 = p + norm1;
        QVector2D p21 = p + norm2;
        QVector2D p22 = _currentRoute[i+1].toVec2D() + norm2;
        QVector2D inters = lineIntersection(p11, p12, p21, p22);
        norm = inters - p;
      }
    }

    ps2.push_back( p.x() + norm.x() );
    ps2.push_back( p.y() + norm.y() );

    ps3.push_back( p.x() - norm.x() );
    ps3.push_back( p.y() - norm.y() );
  }

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[0][ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, ps1.size() * sizeof(GLfloat), ps1.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[1][ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, ps2.size() * sizeof(GLfloat), ps2.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[2][ATTR_COORDS]);
  glBufferData(GL_ARRAY_BUFFER, ps3.size() * sizeof(GLfloat), ps3.data(), GL_DYNAMIC_DRAW);

  return ps1.size() / 2;
}


QVector2D RouteEngine::lineIntersection( const QVector2D& p11, const QVector2D& p12
                                       , const QVector2D& p21, const QVector2D& p22) const {
  float d1 = (p11.x()*p12.y()-p11.y()*p12.x());
  float d2 = (p21.x()*p22.y()-p21.y()*p22.x());
  float d3 = (p11.x()-p12.x())*(p21.y()-p22.y()) - (p11.y()-p12.y())*(p21.x()-p22.x());

  float xt = d1*(p21.x()-p22.x()) - d2*(p11.x()-p12.x());
  float yt = d1*(p21.y()-p22.y()) - d2*(p11.y()-p12.y());

  return QVector2D(xt/d3, yt/d3);
}

