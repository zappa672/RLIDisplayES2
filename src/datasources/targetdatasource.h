#ifndef TARGETDATASOURCE_H
#define TARGETDATASOURCE_H

#include "datasourcebase.h"

#include "../common/structures.h"

#include <QVector>

namespace RLI {

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
