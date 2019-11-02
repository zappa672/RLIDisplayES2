#ifndef RADARDATASOURCE_H
#define RADARDATASOURCE_H

#include "datasourcebase.h"

#include <QOpenGLFunctions>

namespace RLI {

  class RadarDataSource : public DataSourceBase {
    Q_OBJECT
  public:
    explicit RadarDataSource(int period, QObject* parent = nullptr);
    virtual ~RadarDataSource();

  signals:
    void updateRadarData(int offset, int count, GLfloat* amps);
    void updateTrailData(int offset, int count, GLfloat* amps);

  protected slots:
    void timerEvent(QTimerEvent* e);

  private:
    void loadData();
    void loadObserves(GLfloat* amps, char* filename);
    void initDummy(GLfloat* amps, std::function<GLfloat(int, int)> func);

    GLfloat* file_amps1[2];
    GLfloat* file_amps2[2];

    int _blocks_to_send;

    int _file = 0;
    int _offset = 0;

    int _peleng_size;
    int _bearings_per_cycle;
  };

}

#endif // RADARDATASOURCE_H

