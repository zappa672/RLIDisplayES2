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



bool RadarDataSource::loadData() {
  if (!initWithDummy1(file_amps1[0]))
    return false;

  if (!initWithDummy2(file_amps1[1]))
    return false;

  if (!initWithDummy3(file_amps2[1]))
    return false;

  if (!initWithDummy4(file_amps2[0]))
    return false;

  return true;
}

bool RadarDataSource::initWithDummy1(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (i % 256 < 9 || i % 256 > 247)
        amps[i*_peleng_size+j] = (255.f * j) / _peleng_size;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}

bool RadarDataSource::initWithDummy2(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (j > 259 && j < 268)
        amps[i*_peleng_size+j] = 255.f - (255.f * i) / _bearings_per_cycle;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}

bool RadarDataSource::initWithDummy3(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (i % 256 < 137 && i % 256 > 121)
        amps[i*_peleng_size+j] = (255.f * j) / _peleng_size;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}

bool RadarDataSource::initWithDummy4(GLfloat* amps) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      if (j > 131 && j < 140)
        amps[i*_peleng_size+j] = 255.f - (255.f * i) / _bearings_per_cycle;
      else
        amps[i*_peleng_size+j] = 0.f;

  return true;
}
