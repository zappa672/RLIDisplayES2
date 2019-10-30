#ifndef TARGETDATASOURCE_H
#define TARGETDATASOURCE_H

#include "datasourcebase.h"
#include <QVector>

namespace RLI {

  struct Target {
    Target() { }

    Target(const Target& o) :
      lost        { o.lost },
      latitude    { o.latitude },
      longtitude  { o.longtitude },
      heading     { o.heading },
      rotation    { o.rotation },
      course_grnd { o.course_grnd },
      speed_grnd  { o.speed_grnd }
    { }

    ~Target() { }

    bool lost          { false };
    double latitude    { 0 };
    double longtitude  { 0 };
    double heading     { 0 };
    double rotation    { 0 };
    double course_grnd { 0 };
    double speed_grnd  { 0 };
  };

  class TargetDataSource : public DataSourceBase
  {
    Q_OBJECT
  public:
    explicit TargetDataSource(int period, QObject *parent = nullptr);
    virtual ~TargetDataSource();

  signals:
    void updateTarget(const QString& tag, const Target& target);

  protected slots:
    void timerEvent(QTimerEvent* e);

  private:
    QVector<Target> _targets;
  };

}

#endif // TARGETDATASOURCE_H


/*
enum {
  TAILMODE_FIRST  = 0,
  TAILMODE_OFF    = 0,
  TAILMODE_RADAR  = 1,
  TAILMODE_DOTS   = 2,
  TAILMODE_LAST   = 2
};

void incrementMode();
void onTailsModeChanged(const QByteArray mode);
void tailsModeChanged(int mode, int minutes);
*/
