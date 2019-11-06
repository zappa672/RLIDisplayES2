#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QVector2D>

namespace RLI {

  struct GeoPos {
    double lat, lon;

    inline QVector2D toVec2D() {
      return QVector2D { static_cast<float>(lat), static_cast<float>(lon) };
    }

    GeoPos(double latitude, double longtitude): lat {latitude}, lon {longtitude} {}
    GeoPos(const QVector2D& v): GeoPos(static_cast<double>(v.x()), static_cast<double>(v.y())) {}
  };

  struct ShipState {
    GeoPos position  { 0.0, 0.0 };
    double course    { 0.0 };
    double speed     { 0.0 };
  };

  struct Target {
    int     id          { 0 };
    bool    lost        { false };
    GeoPos  position    { 0.0, 0.0 };
    double  heading     { 0 };
    double  rotation    { 0 };
    double  course_grnd { 0 };
    double  speed_grnd  { 0 };
  };

}

#endif // STRUCTURES_H
