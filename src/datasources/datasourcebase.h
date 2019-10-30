#ifndef DATASOURCEBASE_H
#define DATASOURCEBASE_H

#include <QObject>
#include <QTimerEvent>
#include <QDateTime>

namespace RLI {
  class DataSourceBase : public QObject
  {
    Q_OBJECT
  public:
    explicit DataSourceBase(int period, QObject* parent = nullptr);
    virtual ~DataSourceBase();

    inline long elapsed() { return _timerId == -1 ? 0 : _startTime.msecsTo(QDateTime::currentDateTime()); }

  public slots:
    void start();
    void stop();

  protected slots:
    virtual void timerEvent(QTimerEvent*) {}

  private:
    int _timer_period;
    QDateTime _startTime;
    int _timerId = -1;
  };
}


#endif // DATASOURCEBASE_H
