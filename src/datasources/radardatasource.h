#ifndef RADARDATASOURCE_H
#define RADARDATASOURCE_H

#include <stdint.h>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

#include <QtGlobal>
#include <QOpenGLFunctions>

namespace RLI {

  class RadarDataSource : public QObject {
    Q_OBJECT
  public:
    explicit RadarDataSource(QObject* parent = nullptr);
    virtual ~RadarDataSource();

  public slots:
    void start();
    void stop();

  signals:
    void updateRadarData(uint offset, uint count, GLfloat* amps);
    void updateTrailData(uint offset, uint count, GLfloat* amps);

  protected slots:
    void timerEvent(QTimerEvent* e);

  private:
    bool loadData();

    bool loadObserves1(char* filename, GLfloat* amps);

    bool initWithDummy1(float* amps);
    bool initWithDummy2(float* amps);
    bool initWithDummy3(float* amps);
    bool initWithDummy4(float* amps);

    int _timerId = -1;

    GLfloat* file_amps1[2];
    GLfloat* file_amps2[2];

    int _timer_period;
    uint _blocks_to_send;

    uint _file = 0;
    uint _offset = 0;

    uint _peleng_size;
    uint _bearings_per_cycle;
  };

}

#endif // RADARDATASOURCE_H

