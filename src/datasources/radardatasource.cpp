#include "radardatasource.h"

#include "../common/properties.h"

#include <QApplication>
#include <QtConcurrent/QtConcurrentRun>

using namespace RLI;

RadarDataSource::RadarDataSource(int period, QObject* parent) : DataSourceBase(period, parent) {
  _blocks_to_send      = qApp->property(PROPERTY_BLOCK_SIZE).toInt();
  _peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  _bearings_per_cycle  = qApp->property(PROPERTY_PELENG_COUNT).toInt();

  file_amps1[0] = new GLfloat[_peleng_size*_bearings_per_cycle];
  file_amps1[1] = new GLfloat[_peleng_size*_bearings_per_cycle];
  file_amps2[0] = new GLfloat[_peleng_size*_bearings_per_cycle];
  file_amps2[1] = new GLfloat[_peleng_size*_bearings_per_cycle];

  loadData();
}

RadarDataSource::~RadarDataSource() {
  stop();

  delete file_amps1[0];
  delete file_amps1[1];
  delete file_amps2[0];
  delete file_amps2[1];
}



void RadarDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e)

  QtConcurrent::run([&](int offset, int file) {
    emit updateRadarData(offset, _blocks_to_send, &file_amps1[file][offset * _peleng_size]);
    emit updateTrailData(offset, _blocks_to_send, &file_amps2[file][offset * _peleng_size]);
  }, _offset, _file);

  _offset = (_offset + _blocks_to_send) % _bearings_per_cycle;
  if (_offset == 0) _file = 1 - _file;
}



void RadarDataSource::loadData() {
  initDummy(file_amps1[0], [&](int i, int j) {
      return (i % 256 < 9 || i % 256 > 247) ? (255.f * j) / _peleng_size : 0.f;
    });

  initDummy(file_amps1[1], [&](int i, int j) {
      return (j > 259 && j < 268) ? 255.f - (255.f * i) / _bearings_per_cycle : 0.f;
    });

  initDummy(file_amps2[1], [&](int i, int j) {
      return (i % 256 < 137 && i % 256 > 121) ? (255.f * j) / _peleng_size : 0.f;
    });

  initDummy(file_amps2[0], [&](int i, int j) {
      return (j > 131 && j < 140) ? 255.f - (255.f * i) / _bearings_per_cycle : 0.f;
    });
}


bool loadObserves(GLfloat* amps, char* filename) {

}

void RadarDataSource::initDummy(GLfloat* amps, std::function<float(int, int)> func) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      amps[i*_peleng_size+j] = func(i, j);
}
