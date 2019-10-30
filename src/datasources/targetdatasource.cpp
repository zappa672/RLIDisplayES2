#include "targetdatasource.h"

#include <cmath>

using namespace RLI;

TargetDataSource::TargetDataSource(int period, QObject *parent) : DataSourceBase(period, parent) {
  Target trgt;

  trgt.lost         = false;
  trgt.latitude     = 15.4200;
  trgt.longtitude   = 145.7600;
  trgt.heading      = 37.0;
  trgt.rotation     = 20.0;
  trgt.course_grnd  = 37.0;
  trgt.speed_grnd   = 180.0;

  _targets.push_back(trgt);

  trgt.latitude     = 15.1500;
  trgt.longtitude   = 145.8600;
  trgt.heading      = 123.0;
  trgt.rotation     = -20.0;
  trgt.course_grnd  = 123.0;
  trgt.speed_grnd   = 80.0;

  _targets.push_back(trgt);

  trgt.latitude     = 15.3500;
  trgt.longtitude   = 145.5600;
  trgt.heading      = 286.0;
  trgt.rotation     = 0.0;
  trgt.course_grnd  = 286.0;
  trgt.speed_grnd   = 140.0;

  _targets.push_back(trgt);

  trgt.latitude     = 15.3000;
  trgt.longtitude   = 144.9300;
  trgt.heading      = -1.0;
  trgt.rotation     = 0.0;
  trgt.course_grnd  = 286.0;
  trgt.speed_grnd   = 140.0;

  _targets.push_back(trgt);
}

TargetDataSource::~TargetDataSource() {
  stop();
}

const float PI = 3.14159265359f;

void TargetDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e)

  QDateTime now = QDateTime::currentDateTime();

  for (int i = 0; i < _targets.size(); i++) {
    Target target;

    double e = DataSourceBase::elapsed();

    target.longtitude = _targets[i].longtitude + 0.02f * (i+1) * sin(e/(2000.f*(i+1)) + i);
    target.latitude = _targets[i].latitude + 0.02f * (i+1) * cos(e/(2000.f*(i+1)) + i);
    target.course_grnd = int(450 + 180 * (e/(2000.f*(i+1)) + i) / PI) % 360;
    if (_targets[i].heading != -1)
      target.heading = int(360 + target.course_grnd - target.rotation) % 360;
    else
      target.heading = -1;
    target.speed_grnd = _targets[i].speed_grnd;

    emit updateTarget(QString::number(i+1), target);
  }
}


/*
tail_mode    = TAILMODE_OFF;
tail_minutes = 1;

void TargetDataSource::incrementMode() {
  if (++tail_mode > TAILMODE_LAST)
    tail_mode = TAILMODE_FIRST;

  emit tailsModeChanged(tail_mode, (tail_mode == TAILMODE_DOTS) ? tail_minutes : 0);
}

void TargetDataSource::onTailsModeChanged(const QByteArray mode) {
  tail_minutes = atoi(mode.data());

  emit tailsModeChanged(tail_mode, (tail_mode == TAILMODE_DOTS) ? tail_minutes : 0);
}
*/
