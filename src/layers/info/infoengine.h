#ifndef INFOENGINE_H
#define INFOENGINE_H

#include <map>
#include <array>

#include <QRect>
#include <QColor>

#include "../../common/layout.h"
#include "../../common/state.h"
#include "../../common/strings.h"

#include "../fonts.h"

#include "infoblock.h"

namespace RLI {

  class InfoEngine : public QObject {
    Q_OBJECT

  public:
    InfoEngine(const Layout& layout, QOpenGLContext* context, const Fonts* fonts, QObject* parent = nullptr);
    virtual ~InfoEngine();

    void resize(const Layout& layout);
    void update(const State& state, const Layout& layout);

    inline const std::map<PanelId, InfoBlock*>& blocks() { return _blocks; }

  public slots:
    void needFullUpdate();

    void updateGain(float gain);
    void updateWater(float water);
    void updateRain(float rain);
    void updateApch(float apch);
    void updateEmission(float emission);

    void updateValueBar(PanelId id, float value);

    void secondChanged();
    void setFps(int fps);

    void onCourseChanged(double course);
    void onPositionChanged(const GeoPos& position);
    void onTargetCountChanged(int count);
    void onSelectedTargetUpdated(const QString& tag, const Target& trgt);
    void onScaleChanged(const RadarScale* scale);
    void onOrientationChanged(Orientation orient);
    void onVnChanged(const State& state);
    void onVdChanged(const State& state);
    void onCursorPosChanged(double dist, double angle);

  private:
    void initBlocks();

    void initBlockGain();
    void initBlockWater();
    void initBlockRain();
    void initBlockApch();
    void initBlockEmission();

    void initBlockLabel5();
    void initBlockBand();
    void initBlockOrientation();
    void initBlockLabel2();
    void initBlockLabel3();
    void initBlockLabel4();

    void initBlockFps();
    void initBlockScale();
    void initBlockVn();
    void initBlockVd();
    void initBlockCourse();
    void initBlockPosition();
    void initBlockBlank();
    void initBlockClock();
    void initBlockDanger();
    void initBlockTails();
    void initBlockDetails();
    void initBlockVector();
    void initBlockTargets();
    void initBlockCursor();

    std::map<PanelId, InfoBlock*> _blocks;
    // -----------------------------------------------
  };

}

#endif // INFOENGINE_H

