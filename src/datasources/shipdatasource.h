#ifndef SHIPDATASOURCE_H
#define SHIPDATASOURCE_H

#include "datasourcebase.h"

#include "../common/structures.h"

namespace RLI {

  class ShipDataSource : public DataSourceBase
  {
    Q_OBJECT
  public:
    explicit ShipDataSource(int period, QObject *parent = nullptr);
    virtual ~ShipDataSource();

    inline const ShipState& shipState() const { return _ship_state; }

  signals:
    void shipStateChanged(ShipState state);

  protected slots:
    void timerEvent(QTimerEvent* e);

  private:
    ShipState _ship_state;
  };

}

#endif // SHIPDATASOURCE_H
