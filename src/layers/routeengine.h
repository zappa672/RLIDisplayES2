#ifndef ROUTEENGINE_H
#define ROUTEENGINE_H

#define ROUTES_COUNT 4


#include <QList>
#include <QMutex>
#include <QVector2D>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

#include "../common/state.h"

#include "layerbase.h"

namespace RLI {

  class RouteEngine : public LayerBase {
    Q_OBJECT

  public:
    explicit RouteEngine(QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~RouteEngine() override;

    void paint(const State& state, const Layout& layout) override;

    GeoPos getLastPoint() { return _currentRoute.last(); }
    inline bool isIndexUsed(int index) { return (index < 0 || index >= 4 || _routes[index].size() > 0); }

  public slots:
    void clearCurrentRoute();
    void addPointToCurrent(const GeoPos& p);
    void removePointFromCurrent();

    void loadFrom(int index);
    void saveTo(int index);

  private:
    int loadBuffers();

    QVector2D lineIntersection( const QVector2D& p11, const QVector2D& p12
                              , const QVector2D& p21, const QVector2D& p22) const;

    QMutex _routesMutex;
    QList<GeoPos> _currentRoute;
    bool _need_reload_buffer = false;
    QList<GeoPos> _routes[ROUTES_COUNT];

    // -----------------------------------------------
    enum SHADER_ATTRIBUTES
    { ATTR_COORDS  = 0
    , ATTR_COUNT   = 1 };
    enum SHADER_UNIFORMS
    { UNIF_MVP_MATRIX  = 0
    , UNIF_CENTER      = 1
    , UNIF_SCALE       = 2 };

    GLuint _vbo_ids[3][ATTR_COUNT];
  };
}

#endif // ROUTEENGINE_H
