#include "radardatasource.h"

#include "../common/properties.h"

#include <iostream>
#include <fstream>

#include <QApplication>
#include <QtConcurrent/QtConcurrentRun>

using namespace RLI;

RadarDataSource::RadarDataSource(int period, QObject* parent) : DataSourceBase(period, parent) {
  _blocks_to_send      = qApp->property(PROPERTY_BLOCK_SIZE).toInt();
  _peleng_size         = qApp->property(PROPERTY_PELENG_SIZE).toInt();
  _bearings_per_cycle  = qApp->property(PROPERTY_PELENG_COUNT).toInt();

  for (int i = 0; i < 4; i++) {
    _amps[i] = new GLfloat[_peleng_size*_bearings_per_cycle];
    std::fill_n(_amps[i], _peleng_size*_bearings_per_cycle, 0.f);
  }

  loadData();
}

RadarDataSource::~RadarDataSource() {
  stop();

  for (int i = 0; i < 4; i++)
    delete _amps[i];
}



void RadarDataSource::timerEvent(QTimerEvent* e) {
  Q_UNUSED(e)

  QtConcurrent::run([&](int offset, int file) {
    emit updateRadarData(offset, _blocks_to_send, &_amps[file][offset * _peleng_size]);
    emit updateTrailData(offset, _blocks_to_send, &_amps[file+2][offset * _peleng_size]);
  }, _offset, _file);

  _offset = (_offset + _blocks_to_send) % _bearings_per_cycle;
  if (_offset == 0) _file = 1 - _file;
}



void RadarDataSource::loadData() {
  loadObserves(_amps[0], "assets/pelengs/r1nm3h0_4096");
  loadObserves(_amps[1], "assets/pelengs/r1nm6h0_4096");

  //initDummy(file_amps1[0], [&](int i, int j) {
  //    return (i % 256 < 9 || i % 256 > 247) ? (255.f * j) / _peleng_size : 0.f;
  //  });
  //
  //initDummy(file_amps1[1], [&](int i, int j) {
  //    return (j > 259 && j < 268) ? 255.f - (255.f * i) / _bearings_per_cycle : 0.f;
  //  });

  initDummy(_amps[2], [&](int i, int j) {
      return (i % 256 < 137 && i % 256 > 121) ? (255.f * j) / _peleng_size : 0.f;
    });

  initDummy(_amps[3], [&](int i, int j) {
      return (j > 131 && j < 140) ? 255.f - (255.f * i) / _bearings_per_cycle : 0.f;
    });
}


void RadarDataSource::loadObserves(GLfloat* amps, const char* filename) {
  std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

  // 16 and 3204 in bytes, we will use INT16
  const int headerSize = 16 / 2;
  const int dataSize = 3204 / 2;

  if (file.is_open()) {
    std::streampos size = file.tellg();
    int16_t* memblock = new int16_t[size/2];

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(memblock), size);
    file.close();

    // Works only for _bearings_per_cycle = 4096, _peleng_size = 800
    for (int i = 0; i < _bearings_per_cycle; i++) {
      float div = memblock[headerSize + i*dataSize + 1];
      for (int j = 0; j < _peleng_size; j++) {
        amps[i*_peleng_size + j] = memblock[headerSize + i*dataSize + 2 + j] / div;
      }
    }

    delete[] memblock;
  } else
    std::cerr << "Unable to open file" << std::endl;
}

void RadarDataSource::initDummy(GLfloat* amps, std::function<float(int, int)> func) {
  for (int i = 0; i < _bearings_per_cycle; i++)
    for (int j = 0; j < _peleng_size; j++)
      amps[i*_peleng_size+j] = func(i, j);
}
