#ifndef RLISTATE_H
#define RLISTATE_H

#include <QObject>
#include <QPoint>

#include "radarscale.h"
#include "math.h"

namespace RLI {

  enum class DepthUnit {
    FEET    = 0
  , METER   = 1
  , FATHOM  = 2 };

  enum class WidgetState {
    DEFAULT        = 0
  , MAIN_MENU      = 1
  , CONFIG_MENU    = 2
  , MAGNIFIER      = 3
  , ROUTE_EDITION  = 4
  };

  enum class Orientation {
    HEAD     = 0
  , NORTH    = 1
  , COURSE   = 2 };

  enum class Mode : char {
    T   = 'T'
  , S   = 'S'
  , X   = 'X'
  , M   = 'M' } ;


  struct State {
    DepthUnit   depth_unit   { DepthUnit::METER };
    WidgetState state        { WidgetState::DEFAULT };
    Orientation orientation  { Orientation::NORTH };
    Mode        mode         { Mode::X };

    float gain                { 0.f };
    float water               { 0.f };
    float rain                { 0.f };
    float apch                { 0.f };
    float emission            { 0.f };

    // Radar parameters

    // Chart parameters
    double      north_shift   { 45.0 };
    RadarScale  radar_scale   { };
    QPoint      center_shift  { 100, 100 };

    // Ship parameters
    GeoPos  ship_position     { 0, 0 };
    double  ship_speed        { 0 };
    double  ship_course       { 0 };

    // Controls parameters
    GeoPos  visir_center_pos  { 0, 0 };
    double  vn_p              { 0 };
    double  vn_cu             { 0 };
    double  course_mark_angle { 0 };
    double  vd                { 0 };
    QPoint  cursor_pos        { 0, 0 };
    bool    show_circles      { false };
    bool    show_parallel     { false };

    // Radar parameters
    int peleng_count    { 4096 };
    int peleng_size     { 800  };

    // Magnifier parameters
    int magn_min_rad    { 96  };
    int magn_min_peleng { 90  };
    int magn_height     { 224 };
    int magn_width      { 224 };
  };
}

#endif // RLISTATE_H
