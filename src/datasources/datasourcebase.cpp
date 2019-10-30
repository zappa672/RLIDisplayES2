#include "datasourcebase.h"

using namespace RLI;

DataSourceBase::DataSourceBase(int period, QObject* parent) : QObject(parent) {
  _timer_period = period;
}

DataSourceBase::~DataSourceBase() {
  stop();
}

#include <QTimer>

void DataSourceBase::start() {
  if (_timerId == -1)
    _timerId = startTimer(_timer_period, Qt::PreciseTimer);

  _startTime = QDateTime::currentDateTime();
}

void DataSourceBase::stop() {
  if (_timerId != -1) {
    killTimer(_timerId);
    _timerId = -1;
  }
}
