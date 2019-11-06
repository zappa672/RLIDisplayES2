#include "infoengine.h"

#include <QTime>

#include "../../common/properties.h"

using namespace RLI;

InfoEngine::InfoEngine(const Layout& layout, QOpenGLContext* context, const Fonts* fonts, QObject* parent) : QObject(parent)  {
  for (PanelId name: layout.panels.keys())
    _blocks.emplace(name, new InfoBlock(name, layout, context, fonts, this));

  initBlocks();
}

InfoEngine::~InfoEngine() {
  for (auto block: _blocks)
    delete block.second;
}

void InfoEngine::needFullUpdate() {
  std::for_each(_blocks.begin(), _blocks.end(), [](std::pair<PanelId, InfoBlock*> b) { b.second->forceUpdate(); });
  for (auto b: _blocks)
    b.second->forceUpdate();
}

void InfoEngine::resize(const Layout& layout) {
  for (auto block: _blocks)
    block.second->resizeTexture(layout);

  initBlocks();  
}

void InfoEngine::update(const State& state, const Layout& layout) {
  glEnable(GL_BLEND);

  for (std::pair<PanelId, InfoBlock*> b: _blocks)
    b.second->paint(state, layout);
}


void InfoEngine::initBlocks() {
  initBlockGain();
  initBlockWater();
  initBlockRain();
  initBlockApch();
  initBlockEmission();

  initBlockLabel5();
  initBlockBand();
  initBlockOrientation();
  initBlockLabel2();
  initBlockLabel3();
  initBlockLabel4();

  initBlockFps();
  initBlockScale();
  initBlockVn();
  initBlockVd();
  initBlockCourse();
  initBlockPosition();
  initBlockBlank();
  initBlockClock();
  initBlockDanger();
  initBlockTails();
  initBlockDetails();
  initBlockVector();
  initBlockTargets();
  initBlockCursor();
}


void InfoEngine::initBlockGain() {
  _blocks[PanelId::GAIN]->setText(PNL_GAIN_TEXT_ID, StrId::GAIN);
}

void InfoEngine::initBlockWater() {
  _blocks[PanelId::WATER]->setText(PNL_WATER_TEXT_ID, StrId::WATER);
}

void InfoEngine::initBlockRain() {
  _blocks[PanelId::RAIN]->setText(PNL_RAIN_TEXT_ID, StrId::RAIN);
}

void InfoEngine::initBlockApch() {
  _blocks[PanelId::APCH]->setText(PNL_APCH_TEXT_ID, StrId::AFC);
}

void InfoEngine::initBlockEmission() {
  _blocks[PanelId::EMISSION]->setText(PNL_EMISSION_TEXT_ID, StrId::EMISSION);
}

void InfoEngine::updateGain(float gain) {
  updateValueBar(PanelId::GAIN, gain);
}

void InfoEngine::updateWater(float water) {
  updateValueBar(PanelId::WATER, water);
}

void InfoEngine::updateRain(float rain) {
  updateValueBar(PanelId::RAIN, rain);
}

void InfoEngine::updateApch(float apch) {
  updateValueBar(PanelId::APCH, apch);
}

void InfoEngine::updateEmission(float emission) {
  updateValueBar(PanelId::EMISSION, emission);
}

void InfoEngine::updateValueBar(PanelId id, float value) {
  int max_bar_width = _blocks[id]->width() - _blocks[id]->rect("splitter").geometry.x() - 2;
  int bar_width = static_cast<int>((value / 255.f) * max_bar_width);

  QRect bar = _blocks[id]->rect("bar").geometry;

  bar.setWidth(bar_width);
  _blocks[id]->setRect("bar", bar);
}



void InfoEngine::initBlockLabel5() {
  _blocks[PanelId::LABEL5]->setText(PNL_LABEL5_TEXT_ID, StrId::PP12p);
}

void InfoEngine::initBlockBand() {
  _blocks[PanelId::BAND]->setText(PNL_BAND_TEXT_ID, StrId::S_BAND);
}


void InfoEngine::initBlockOrientation() {
  _blocks[PanelId::ORIENTATION]->setText(PNL_ORIENTATION_TEXT_ID, StrId::NORTH);
}

void InfoEngine::initBlockVn() {
  _blocks[PanelId::VN]->setText(PNL_VN_HEADER_TEXT_ID, StrId::EBL);

  _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_0_TEXT_ID, StrId::B);
  _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_2_TEXT_ID, StrId::DEGREE_SIGN);

  _blocks[PanelId::VN]->setText(PNL_VN_TBL_1_0_TEXT_ID, StrId::CU);
  _blocks[PanelId::VN]->setText(PNL_VN_TBL_1_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::VN]->setText(PNL_VN_TBL_1_2_TEXT_ID, StrId::GRAD_RB);
}

void InfoEngine::onVnChanged(const State& state) {
  if (state.orientation == Orientation::NORTH)
    _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_1_TEXT_ID, QString::number(state.vn_p, 'd', 2).left(5).toLocal8Bit());

  _blocks[PanelId::VN]->setText(PNL_VN_TBL_1_1_TEXT_ID, QString::number(state.vn_cu, 'd', 2).left(5).toLocal8Bit());
}

void InfoEngine::initBlockCursor() {
  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_HEADER_TEXT_ID, StrId::CURSOR);

  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_0_0_TEXT_ID, StrId::BEARING);
  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_0_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_0_2_TEXT_ID, StrId::DEGREE_SIGN);

  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_1_0_TEXT_ID, StrId::RANGE);
  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_1_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_1_2_TEXT_ID, StrId::NM);
}

void InfoEngine::onCursorPosChanged(double dist, double angle) {
  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_0_1_TEXT_ID, QString::number(angle, 'd', 2).left(6).toLocal8Bit());
  _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_1_1_TEXT_ID, QString::number(dist, 'd', 2).left(5).toLocal8Bit());
}

void InfoEngine::onOrientationChanged(Orientation orient) {
  switch (orient) {
    case Orientation::HEAD:
      _blocks[PanelId::ORIENTATION]->setText(PNL_ORIENTATION_TEXT_ID, StrId::HEAD);
      break;
    case Orientation::NORTH:
      _blocks[PanelId::ORIENTATION]->setText(PNL_ORIENTATION_TEXT_ID, StrId::NORTH);
      break;
    case Orientation::COURSE:
      _blocks[PanelId::ORIENTATION]->setText(PNL_ORIENTATION_TEXT_ID, StrId::COURSE);
      break;
  }

  if (orient == Orientation::NORTH) {
    _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_0_TEXT_ID, StrId::B);
    _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_1_TEXT_ID, QByteArray("0.00"));
    _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_2_TEXT_ID, StrId::DEGREE_SIGN);

    _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_0_0_TEXT_ID, StrId::BEARING);
  } else {
    _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_0_TEXT_ID, QByteArray(""));
    _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_1_TEXT_ID, QByteArray(""));
    _blocks[PanelId::VN]->setText(PNL_VN_TBL_0_2_TEXT_ID, QByteArray(""));

    _blocks[PanelId::CURSOR]->setText(PNL_CURSOR_TBL_0_0_TEXT_ID, StrId::HB);
  }
}



void InfoEngine::initBlockLabel2() {
  _blocks[PanelId::LABEL2]->setText(PNL_LABEL2_TEXT_ID, StrId::RM);
}

void InfoEngine::initBlockLabel3() {
  _blocks[PanelId::LABEL3]->setText(PNL_LABEL3_TEXT_ID, StrId::WATER);
}

void InfoEngine::initBlockLabel4() {
  _blocks[PanelId::LABEL4]->setText(PNL_LABEL4_TEXT_ID, StrId::LOD);
}



void InfoEngine::initBlockFps() {
  _blocks[PanelId::FPS]->setText(PNL_FPS_LABEL_TEXT_ID, StrId::FPS);
  _blocks[PanelId::FPS]->setText(PNL_FPS_VALUE_TEXT_ID, QByteArray("0"));
}

void InfoEngine::setFps(int fps) {
  _blocks[PanelId::FPS]->setText(PNL_FPS_VALUE_TEXT_ID, QString::number(fps).toLocal8Bit());
}



void InfoEngine::initBlockScale() {
  _blocks[PanelId::SCALE]->setText(PNL_SCALE_SCALE1_TEXT_ID, QByteArray("0"));
  _blocks[PanelId::SCALE]->setText(PNL_SCALE_SLASH_TEXT_ID, QByteArray("/"));
  _blocks[PanelId::SCALE]->setText(PNL_SCALE_SCALE2_TEXT_ID, QByteArray("0"));
  _blocks[PanelId::SCALE]->setText(PNL_SCALE_UNITS_TEXT_ID, StrId::NM);
}

void InfoEngine::onScaleChanged(const RadarScale* scale) {
  _blocks[PanelId::SCALE]->setText(PNL_SCALE_SCALE1_TEXT_ID, QByteArray(scale->current()->display));
  _blocks[PanelId::SCALE]->setText(PNL_SCALE_SCALE2_TEXT_ID, QByteArray(scale->current()->dist_rng_display));
}



void InfoEngine::initBlockVd() {
  _blocks[PanelId::VD]->setText(PNL_VD_HEADER_TEXT_ID, StrId::VRM);

  _blocks[PanelId::VD]->setText(PNL_VD_TBL_0_0_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::VD]->setText(PNL_VD_TBL_0_1_TEXT_ID, StrId::NM);
}

void InfoEngine::onVdChanged(const State& state) {
  _blocks[PanelId::VD]->setText(PNL_VD_TBL_0_0_TEXT_ID, QString::number(state.vd, 'd', 2).left(5).toLocal8Bit());
}



void InfoEngine::initBlockCourse() {
  _blocks[PanelId::COURSE]->setText(PNL_COURSE_TBL_0_0_TEXT_ID, StrId::GYRO_HDG);
  _blocks[PanelId::COURSE]->setText(PNL_COURSE_TBL_0_1_TEXT_ID, QByteArray("0"));
  _blocks[PanelId::COURSE]->setText(PNL_COURSE_TBL_0_2_TEXT_ID, StrId::DEGREE_SIGN);

  _blocks[PanelId::COURSE]->setText(PNL_COURSE_TBL_1_0_TEXT_ID, StrId::MAN_SPD);
  _blocks[PanelId::COURSE]->setText(PNL_COURSE_TBL_1_1_TEXT_ID, QByteArray("0"));
  _blocks[PanelId::COURSE]->setText(PNL_COURSE_TBL_1_2_TEXT_ID, StrId::NM);
}

void InfoEngine::onCourseChanged(double course) {
  _blocks[PanelId::COURSE]->setText(PNL_COURSE_TBL_0_1_TEXT_ID, QString::number(course, 'd', 2).left(5).toLocal8Bit());
}


void InfoEngine::initBlockPosition() {
  _blocks[PanelId::POSITION]->setText(PNL_POSITION_TBL_0_0_TEXT_ID, StrId::LAT);
  _blocks[PanelId::POSITION]->setText(PNL_POSITION_TBL_0_1_TEXT_ID, StrId::BLANK);

  _blocks[PanelId::POSITION]->setText(PNL_POSITION_TBL_1_0_TEXT_ID, StrId::LON);
  _blocks[PanelId::POSITION]->setText(PNL_POSITION_TBL_1_1_TEXT_ID, StrId::BLANK);
}

void InfoEngine::onPositionChanged(const GeoPos& position) {
  _blocks[PanelId::POSITION]->setText(PNL_POSITION_TBL_0_1_TEXT_ID, QString::number(position.lat, 'd', 2).toLocal8Bit());
  _blocks[PanelId::POSITION]->setText(PNL_POSITION_TBL_1_1_TEXT_ID, QString::number(position.lon, 'd', 2).toLocal8Bit());
}




void InfoEngine::initBlockBlank() {

}


void InfoEngine::initBlockClock() {
  _blocks[PanelId::CLOCK]->setText(PNL_CLOCK_LABEL_TEXT_ID, StrId::TIME);
  _blocks[PanelId::CLOCK]->setText(PNL_CLOCK_TIME_TEXT_ID, QTime::currentTime().toString().toLocal8Bit());
}

void InfoEngine::secondChanged() {
  _blocks[PanelId::CLOCK]->setText(PNL_CLOCK_TIME_TEXT_ID, QTime::currentTime().toString().toLocal8Bit());
}



void InfoEngine::initBlockDanger() {
  _blocks[PanelId::DANGER]->setText(PNL_DANGER_LABEL_TEXT_ID, StrId::DANGER_TRG);
}

void InfoEngine::initBlockTails() {
  _blocks[PanelId::TAILS]->setText(PNL_TAILS_TBL_0_0_TEXT_ID, StrId::TAILS);
  _blocks[PanelId::TAILS]->setText(PNL_TAILS_TBL_0_1_TEXT_ID, StrId::OFF);
  _blocks[PanelId::TAILS]->setText(PNL_TAILS_TBL_0_2_TEXT_ID, StrId::MIN);
}

void InfoEngine::initBlockDetails() {
  _blocks[PanelId::DANGER_INFO]->setText(PNL_DANGER_INFO_TBL_0_0_TEXT_ID, StrId::CPA);
  _blocks[PanelId::DANGER_INFO]->setText(PNL_DANGER_INFO_TBL_0_1_TEXT_ID, QByteArray("0"));
  _blocks[PanelId::DANGER_INFO]->setText(PNL_DANGER_INFO_TBL_0_2_TEXT_ID, StrId::NM);

  _blocks[PanelId::DANGER_INFO]->setText(PNL_DANGER_INFO_TBL_1_0_TEXT_ID, StrId::TCPA_LIMIT);
  _blocks[PanelId::DANGER_INFO]->setText(PNL_DANGER_INFO_TBL_1_1_TEXT_ID, QByteArray("0"));
  _blocks[PanelId::DANGER_INFO]->setText(PNL_DANGER_INFO_TBL_1_2_TEXT_ID, StrId::MIN);
}

void InfoEngine::initBlockVector() {
  _blocks[PanelId::VECTOR]->setText(PNL_VECTOR_TBL_0_0_TEXT_ID, StrId::VECTOR);
  _blocks[PanelId::VECTOR]->setText(PNL_VECTOR_TBL_0_1_TEXT_ID, QByteArray("20"));
  _blocks[PanelId::VECTOR]->setText(PNL_VECTOR_TBL_0_2_TEXT_ID, StrId::MIN);
}

void InfoEngine::initBlockTargets() {
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_HEADER_TEXT_ID, StrId::TRG_ALL);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_CURRENT_TEXT_ID, QByteArray("0"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_COUNT_TEXT_ID, QByteArray("0"));

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_0_0_TEXT_ID, StrId::BEARING);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_0_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_0_2_TEXT_ID, StrId::DEGREE_SIGN);

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_1_0_TEXT_ID, StrId::RANGE);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_1_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_1_2_TEXT_ID, StrId::NM);

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_2_0_TEXT_ID, StrId::COURSE_W);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_2_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_2_2_TEXT_ID, StrId::DEGREE_SIGN);

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_3_0_TEXT_ID, StrId::SPEED_W);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_3_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_3_2_TEXT_ID, StrId::KTS);

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_4_0_TEXT_ID, StrId::CPA);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_4_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_4_2_TEXT_ID, StrId::NM);

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_5_0_TEXT_ID, StrId::TCPA);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_5_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_5_2_TEXT_ID, StrId::MIN);

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_6_0_TEXT_ID, StrId::DCC);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_6_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_6_2_TEXT_ID, StrId::NM);

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_7_0_TEXT_ID, StrId::TCC);
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_7_1_TEXT_ID, QByteArray("0.00"));
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_7_2_TEXT_ID, StrId::MIN);
}

void InfoEngine::onTargetCountChanged(int count) {
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_COUNT_TEXT_ID, QString::number(count).toLocal8Bit());
}

void InfoEngine::onSelectedTargetUpdated(const QString& tag, const Target& trgt) {
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_CURRENT_TEXT_ID, tag.toLocal8Bit());

  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_2_1_TEXT_ID, QString::number(trgt.course_grnd).left(6).toLatin1());
  _blocks[PanelId::TARGETS]->setText(PNL_TARGETS_TBL_3_1_TEXT_ID, QString::number(trgt.speed_grnd).left(6).toLatin1());
}
