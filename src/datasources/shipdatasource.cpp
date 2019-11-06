#include "shipdatasource.h"

#include <cmath>
#include "../common/math.h"


using namespace RLI;

ShipDataSource::ShipDataSource(int period, QObject *parent) : DataSourceBase(period, parent) {
  //_ship_state.position = GeoPos(13.23 + 0.25, 144.38 + 0.25);
  _ship_state.position = GeoPos(15.123 + 0.25, 145.66);
  _ship_state.course = 90;
  _ship_state.course = 0;
}

ShipDataSource::~ShipDataSource() {
  stop();
}

void ShipDataSource::timerEvent(QTimerEvent*) {
  QDateTime now = QDateTime::currentDateTime();
  double elapsed = DataSourceBase::elapsed()/60000.;

  double lat = 15.123 + 0.25 * cos(elapsed);
  double lon = 145.66 + 0.25 * sin(elapsed);

  _ship_state.position = GeoPos(lat, lon);
  _ship_state.course = fmod(90 + Math::degs(elapsed), 360);

  emit shipStateChanged(_ship_state);
}

