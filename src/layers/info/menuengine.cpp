#include "menuengine.h"
#include "../../common/properties.h"

#include <QColor>

using namespace RLI;


static const QColor MENU_LOCKED_ITEM_COLOR   (0xB4, 0xB4, 0xB4);
static const QColor MENU_DISABLED_ITEM_COLOR (0xFC, 0x54, 0x54);
static const QColor MENU_TEXT_STATIC_COLOR   (0x00, 0xFC, 0xFC);
static const QColor MENU_TEXT_DYNAMIC_COLOR  (0xFC, 0xFC, 0x54);

static const QColor MENU_BORDER_COLOR        (0x40, 0xFC, 0x00);
static const QColor MENU_BACKGRD_COLOR       (0x00, 0x00, 0x00);


MenuEngine::MenuEngine(const Layout& layout, const Fonts* fonts, QOpenGLContext* context, QObject* parent)
  : TextureLayerBase(layout.menu.geometry ,context, parent), _fonts(fonts)  {

  qRegisterMetaType<StrId>("StrId");

  _selected_line = 1;
  _selection_active = false;

  glGenBuffers(ATTR_COUNT, _vbo_ids);

  static const QMap<QString, int> attr_inices
  { { "position", ATTR_POSITION }
  , { "order",    ATTR_ORDER    }
  , { "char_val", ATTR_CHAR_VAL } };

  static const QMap<QString, int> unif_inices
  { { "mvp_matrix", UNIF_MVP    }
  , { "color",      UNIF_COLOR  }
  , { "size",       UNIF_SIZE   } };

  initShaderProgram(":/shaders/info", attr_inices, unif_inices);

  initMainMenuTree();
  initCnfgMenuTree();

  _menu = nullptr;
  _last_action_time = QDateTime::currentDateTime();
  _font_tag = layout.menu.font;
  _need_update = true;
}

void MenuEngine::initCnfgMenuTree() {
  MenuItemMenu* m1 = new MenuItemMenu(StrId::MENU_1, nullptr);

  // --------------------------
  MenuItemMenu* m10 = new MenuItemMenu(StrId::MENU_10, m1);
  m1->add_item(m10);

  MenuItemList* i100 = new MenuItemList(StrId::MENU_100, 4);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_200_MINUS);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_500_MINUS);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_COD_MINUS);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_NMEA);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_COD_PLUS);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_500_PLUS);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_200_PLUS);
  i100->addVariant(StrId::ARRAY_LOG_SIGNAL_GPS);
  m10->add_item(static_cast<MenuItem*>(i100));

  MenuItemInt* i101 = new MenuItemInt(StrId::MENU_101, 0, 255, 0);
  m10->add_item(static_cast<MenuItem*>(i101));

  MenuItemReal* i102 = new MenuItemReal(StrId::MENU_102, -179.9, 179.9, 0.);
  m10->add_item(static_cast<MenuItem*>(i102));

  MenuItemReal* i103 = new MenuItemReal(StrId::MENU_103, 0.0, 359.9, 0.0);
  m10->add_item(static_cast<MenuItem*>(i103));

  MenuItemReal* i104 = new MenuItemReal(StrId::MENU_104, 0.0, 359.9, 0.0);
  m10->add_item(static_cast<MenuItem*>(i104));

  MenuItemList* i105 = new MenuItemList(StrId::MENU_105, 0);
  i105->addVariant(StrId::ARRAY_BAND_X);
  i105->addVariant(StrId::ARRAY_BAND_S);
  i105->addVariant(StrId::ARRAY_BAND_K);
  m10->add_item(static_cast<MenuItem*>(i105));
  connect(i105, SIGNAL(valueChanged(StrId)), this, SIGNAL(bandModeChanged(StrId)), Qt::QueuedConnection);

  MenuItemList* i106 = new MenuItemList(StrId::MENU_106, 0);
  i106->addVariant(StrId::ARRAY_OFFON_OFF);
  i106->addVariant(StrId::ARRAY_OFFON_ON);
  m10->add_item(static_cast<MenuItem*>(i106));

  MenuItemInt* i107 = new MenuItemInt(StrId::MENU_107, 25, 100, 80);
  m10->add_item(static_cast<MenuItem*>(i107));


  // --------------------------
  MenuItemMenu* m11 = new MenuItemMenu(StrId::MENU_11, m1);
  m1->add_item(m11);

  MenuItemReal* i110 = new MenuItemReal(StrId::MENU_110, -5.0, 5.0, -4.4);
  m11->add_item(static_cast<MenuItem*>(i110));

  MenuItemInt* i111 = new MenuItemInt(StrId::MENU_111, 1, 255, 170);
  m11->add_item(static_cast<MenuItem*>(i111));

  MenuItemInt* i112 = new MenuItemInt(StrId::MENU_112, -2048, 2048, 0);
  m11->add_item(static_cast<MenuItem*>(i112));
  connect(i112, SIGNAL(valueChanged(int)), this, SIGNAL(analogZeroChanged(int)), Qt::QueuedConnection);
  analogZeroItem = i112;

  MenuItemInt* i113 = new MenuItemInt(StrId::MENU_113, 1, 255, 25);
  m11->add_item(static_cast<MenuItem*>(i113));

  MenuItemInt* i114 = new MenuItemInt(StrId::MENU_114, 0, 255, 18);
  m11->add_item(static_cast<MenuItem*>(i114));

  MenuItemInt* i115 = new MenuItemInt(StrId::MENU_115, 0, 255, 16);
  m11->add_item(static_cast<MenuItem*>(i115));

  MenuItemInt* i116 = new MenuItemInt(StrId::MENU_116, 0, 4096, 100);
  m11->add_item(static_cast<MenuItem*>(i116));

  MenuItemInt* i117 = new MenuItemInt(StrId::MENU_117, 0, 255, 30);
  m11->add_item(static_cast<MenuItem*>(i117));

  MenuItemInt* i118 = new MenuItemInt(StrId::MENU_118, -20, 20, -2);
  m11->add_item(static_cast<MenuItem*>(i118));

  MenuItemInt* i119 = new MenuItemInt(StrId::MENU_119, -20, 20, -2);
  m11->add_item(static_cast<MenuItem*>(i119));

  MenuItemInt* i1110 = new MenuItemInt(StrId::MENU_11A, -20, 20, 0);
  m11->add_item(static_cast<MenuItem*>(i1110));

  // --------------------------
  MenuItemMenu* m12 = new MenuItemMenu(StrId::MENU_12, m1);
  m1->add_item(m12);

  MenuItemList* i120 = new MenuItemList(StrId::MENU_120, 0);
  i120->addVariant(StrId::ARRAY_OFFON_OFF);
  i120->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i120));

  MenuItemList* i121 = new MenuItemList(StrId::MENU_121, 0);
  i121->addVariant(StrId::ARRAY_OFFON_OFF);
  i121->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i121));

  MenuItemList* i122 = new MenuItemList(StrId::MENU_122, 0);
  i122->addVariant(StrId::ARRAY_OFFON_OFF);
  i122->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i122));

  MenuItemList* i123 = new MenuItemList(StrId::MENU_123, 0);
  i123->addVariant(StrId::ARRAY_OFFON_OFF);
  i123->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i123));

  MenuItemList* i124 = new MenuItemList(StrId::MENU_124, 0);
  i124->addVariant(StrId::ARRAY_OFFON_OFF);
  i124->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i124));

  MenuItemList* i125 = new MenuItemList(StrId::MENU_125, 0);
  i125->addVariant(StrId::ARRAY_OFFON_OFF);
  i125->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i125));

  MenuItemList* i126 = new MenuItemList(StrId::MENU_126, 0);
  i126->addVariant(StrId::ARRAY_OFFON_OFF);
  i126->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i126));

  MenuItemList* i127 = new MenuItemList(StrId::MENU_127, 0);
  i127->addVariant(StrId::ARRAY_OFFON_OFF);
  i127->addVariant(StrId::ARRAY_OFFON_ON);
  m12->add_item(static_cast<MenuItem*>(i127));

  // --------------------------
  MenuItemMenu* m13 = new MenuItemMenu(StrId::MENU_13, m1);
  m1->add_item(m13);

  MenuItemList* i130 = new MenuItemList(StrId::MENU_130, 1);
  i130->addVariant(StrId::ARRAY_OFFON_OFF);
  i130->addVariant(StrId::ARRAY_OFFON_ON);
  m13->add_item(static_cast<MenuItem*>(i130));

  MenuItemList* i131 = new MenuItemList(StrId::MENU_131, 1);
  i131->addVariant(StrId::ARRAY_OFFON_OFF);
  i131->addVariant(StrId::ARRAY_OFFON_ON);
  m13->add_item(static_cast<MenuItem*>(i131));

  MenuItemList* i132 = new MenuItemList(StrId::MENU_132, 1);
  i132->addVariant(StrId::ARRAY_OFFON_OFF);
  i132->addVariant(StrId::ARRAY_OFFON_ON);
  m13->add_item(static_cast<MenuItem*>(i132));

  MenuItemInt* i133 = new MenuItemInt(StrId::MENU_133, 4800, 38400, 4800);
  m11->add_item(static_cast<MenuItem*>(i133));


  // --------------------------
  _cnfg_menu = m1;
}


void MenuEngine::initMainMenuTree() {
  MenuItemMenu* m0 = new MenuItemMenu(StrId::MENU_0, nullptr);

  // --------------------------
  MenuItemMenu* m00 = new MenuItemMenu(StrId::MENU_00, m0);
  m0->add_item(m00);

  MenuItemInt* i000 = new MenuItemInt(StrId::MENU_000, 0, 255, 255);
  connect(i000, SIGNAL(valueChanged(int)), this, SIGNAL(radarBrightnessChanged(int)));
  m00->add_item(static_cast<MenuItem*>(i000));

  MenuItemInt* i001 = new MenuItemInt(StrId::MENU_001, 0, 255, 255);
  m00->add_item(static_cast<MenuItem*>(i001));

  MenuItemInt* i002 = new MenuItemInt(StrId::MENU_002, 0, 255, 255);
  m00->add_item(static_cast<MenuItem*>(i002));

  MenuItemInt* i003 = new MenuItemInt(StrId::MENU_003, 0, 255, 255);
  m00->add_item(static_cast<MenuItem*>(i003));

  MenuItemInt* i004 = new MenuItemInt(StrId::MENU_004, 0, 255, 255);
  m00->add_item(static_cast<MenuItem*>(i004));

  MenuItemInt* i005 = new MenuItemInt(StrId::MENU_005, 0, 255, 255);
  m00->add_item(static_cast<MenuItem*>(i005));

  MenuItemInt* i006 = new MenuItemInt(StrId::MENU_006, 0, 255, 255);
  m00->add_item(static_cast<MenuItem*>(i006));

  MenuItemInt* i007 = new MenuItemInt(StrId::MENU_007, 0, 255, 255);
  m00->add_item(static_cast<MenuItem*>(i007));

  MenuItemList* i008 = new MenuItemList(StrId::MENU_008, 0);
  i008->addVariant(StrId::ARRAY_DAY_DAY);
  i008->addVariant(StrId::ARRAY_DAY_NIGHT);
  m00->add_item(static_cast<MenuItem*>(i008));


  // --------------------------
  MenuItemMenu* m01 = new MenuItemMenu(StrId::MENU_01, m0);
  m0->add_item(m01);

  MenuItemReal* i010 = new MenuItemReal(StrId::MENU_010, 0.01, 8.0, 2.0);
  m01->add_item(static_cast<MenuItem*>(i010));

  MenuItemInt* i011 = new MenuItemInt(StrId::MENU_011, 5, 60, 30);
  m01->add_item(static_cast<MenuItem*>(i011));

  MenuItemInt* i012 = new MenuItemInt(StrId::MENU_012, 5, 60, 30);
  m01->add_item(static_cast<MenuItem*>(i012));

  MenuItemList* i013 = new MenuItemList(StrId::MENU_013, 0);
  i013->addVariant(StrId::ARRAY_TRACK_1);
  i013->addVariant(StrId::ARRAY_TRACK_2);
  i013->addVariant(StrId::ARRAY_TRACK_3);
  i013->addVariant(StrId::ARRAY_TRACK_6);
  i013->addVariant(StrId::ARRAY_TRACK_12);
  m01->add_item(static_cast<MenuItem*>(i013));
  connect(i013, SIGNAL(valueChanged(StrId)), this, SIGNAL(tailsModeChanged(StrId)), Qt::QueuedConnection);

  MenuItemList* i014 = new MenuItemList(StrId::MENU_014, 1);
  i014->addVariant(StrId::ARRAY_OFFON_OFF);
  i014->addVariant(StrId::ARRAY_OFFON_ON);
  m01->add_item(static_cast<MenuItem*>(i014));

  MenuItemAction* i015 = new MenuItemAction(StrId::MENU_015);
  i015->setLocked(true);
  m01->add_item(static_cast<MenuItem*>(i015));

  MenuItemAction* i016 = new MenuItemAction(StrId::MENU_016);
  i016->setLocked(true);
  m01->add_item(static_cast<MenuItem*>(i016));

  MenuItemList* i017 = new MenuItemList(StrId::MENU_017, 0);
  i017->addVariant(StrId::ARRAY_TVEC_AP_WATER);
  i017->addVariant(StrId::ARRAY_TVEC_AP_GRND);
  i017->setLocked(true);
  m01->add_item(static_cast<MenuItem*>(i017));


  // --------------------------
  MenuItemMenu* m02 = new MenuItemMenu(StrId::MENU_02, m0);
  m0->add_item(m02);

  MenuItemInt* i020 = new MenuItemInt(StrId::MENU_020, 0, 255, 5);
  m02->add_item(static_cast<MenuItem*>(i020));

  MenuItemList* i021 = new MenuItemList(StrId::MENU_021, 0);
  i021->addVariant(StrId::ARRAY_VD_KM);
  i021->addVariant(StrId::ARRAY_VD_NM);
  m02->add_item(static_cast<MenuItem*>(i021));

  MenuItemList* i022 = new MenuItemList(StrId::MENU_022, 0);
  i022->addVariant(StrId::ARRAY_SPEED_MAN);
  i022->addVariant(StrId::ARRAY_SPEED_LOG);
  m02->add_item(static_cast<MenuItem*>(i022));

  MenuItemReal* i023 = new MenuItemReal(StrId::MENU_023, 0.0, 90.0, 5.0);
  m02->add_item(i023);

  MenuItemList* i024 = new MenuItemList(StrId::MENU_024, 2);
  i024->addVariant(StrId::ARRAY_DEV_STAB_ATER);
  i024->addVariant(StrId::ARRAY_DEV_STAB_GPS);
  i024->addVariant(StrId::ARRAY_DEV_STAB_DLG);
  i024->addVariant(StrId::ARRAY_DEV_STAB_L_G_W);
  m02->add_item(i024);

  MenuItemInt* i025 = new MenuItemInt(StrId::MENU_025, 0, 90, 0);
  m02->add_item(i025);

  MenuItemList* i026 = new MenuItemList(StrId::MENU_026, 1);
  i026->addVariant(StrId::ARRAY_LANG_ENGL);
  i026->addVariant(StrId::ARRAY_LANG_RUS);
  connect(i026, SIGNAL(valueChanged(StrId)), this, SIGNAL(languageChanged(StrId)), Qt::QueuedConnection);
  m02->add_item(i026);

  MenuItemReal* i027 = new MenuItemReal(StrId::MENU_027, 0.0, 359.9, 0.0);
  m02->add_item(i027);

  MenuItemInt* i028 = new MenuItemInt(StrId::MENU_028, 1, 100, 1);
  m02->add_item(i028);

  MenuItemInt* i029 = new MenuItemInt(StrId::MENU_029, 1, 100, 1);
  i029->setLocked(true);
  m02->add_item(i029);

  MenuItemList* i02A = new MenuItemList(StrId::MENU_02A, 0);
  i02A->addVariant(StrId::ARRAY_OFFON_OFF);
  i02A->addVariant(StrId::ARRAY_OFFON_ON);
  m02->add_item(i02A);


  // --------------------------
  MenuItemMenu* m03 = new MenuItemMenu(StrId::MENU_03, m0);
  m0->add_item(m03);

  MenuItemInt* i030 = new MenuItemInt(StrId::MENU_030, 1, 2, 1);
  m03->add_item(static_cast<MenuItem*>(i030));

  MenuItemList* i031 = new MenuItemList(StrId::MENU_031, 0);
  i031->addVariant(StrId::ARRAY_OFFON_OFF);
  i031->addVariant(StrId::ARRAY_OFFON_ON);
  connect(i031, SIGNAL(valueChanged(StrId)), this, SIGNAL(simulationChanged(StrId)), Qt::QueuedConnection);
  m03->add_item(static_cast<MenuItem*>(i031));

  MenuItemList* i032 = new MenuItemList(StrId::MENU_032, 1);
  i032->addVariant(StrId::ARRAY_OFFON_OFF);
  i032->addVariant(StrId::ARRAY_OFFON_ON);
  m03->add_item(static_cast<MenuItem*>(i032));

  MenuItemList* i033 = new MenuItemList(StrId::MENU_033, 1);
  i033->addVariant(StrId::ARRAY_OFFON_OFF);
  i033->addVariant(StrId::ARRAY_OFFON_ON);
  m03->add_item(static_cast<MenuItem*>(i033));

  MenuItemReal* i034 = new MenuItemReal(StrId::MENU_034, 0.0, 3.0, 0.0);
  m03->add_item(static_cast<MenuItem*>(i034));

  MenuItemList* i035 = new MenuItemList(StrId::MENU_035, 0);
  i035->addVariant(StrId::ARRAY_YESNO_YES);
  i035->addVariant(StrId::ARRAY_YESNO_NO);
  m03->add_item(static_cast<MenuItem*>(i035));

  MenuItemList* i036 = new MenuItemList(StrId::MENU_036, 1);
  i036->addVariant(StrId::ARRAY_YESNO_YES);
  i036->addVariant(StrId::ARRAY_YESNO_NO);
  i036->setEnabled(false);
  m03->add_item(static_cast<MenuItem*>(i036));

  MenuItemList* i037 = new MenuItemList(StrId::MENU_037, 1);
  i037->addVariant(StrId::ARRAY_YESNO_YES);
  i037->addVariant(StrId::ARRAY_YESNO_NO);
  i037->setEnabled(false);
  m03->add_item(static_cast<MenuItem*>(i037));

  MenuItemList* i038 = new MenuItemList(StrId::MENU_038, 1);
  i038->addVariant(StrId::ARRAY_YESNO_YES);
  i038->addVariant(StrId::ARRAY_YESNO_NO);
  i038->setEnabled(false);
  m03->add_item(static_cast<MenuItem*>(i038));


  // --------------------------
  MenuItemMenu* m04 = new MenuItemMenu(StrId::MENU_04, m0);
  m0->add_item(m04);

  MenuItemInt* i040 = new MenuItemInt(StrId::MENU_040, 1, 4, 1);
  m04->add_item(static_cast<MenuItem*>(i040));
  routeLoaderItem = i040;

  MenuItemInt* i041 = new MenuItemInt(StrId::MENU_041, 0, 10, 1);
  m04->add_item(static_cast<MenuItem*>(i041));

  MenuItemAction* i042 = new MenuItemAction(StrId::MENU_042);
  i042->setLocked(true);
  m04->add_item(static_cast<MenuItem*>(i042));

  MenuItemInt* i043 = new MenuItemInt(StrId::MENU_043, 40, 1000, 200);
  m04->add_item(static_cast<MenuItem*>(i043));

  MenuItemAction* i044 = new MenuItemAction(StrId::MENU_044);
  m04->add_item(static_cast<MenuItem*>(i044));
  routeEditItem = i044;

  MenuItemInt* i045 = new MenuItemInt(StrId::MENU_045, 0, 10, 1);
  m04->add_item(static_cast<MenuItem*>(i045));

  MenuItemList* i046 = new MenuItemList(StrId::MENU_046, 0);
  i046->addVariant(StrId::ARRAY_NAME_SYMB_BUOY);
  i046->addVariant(StrId::ARRAY_NAME_SYMB_MILESTONE);
  i046->addVariant(StrId::ARRAY_NAME_SYMB_UNDERWATER_DAMAGE);
  i046->addVariant(StrId::ARRAY_NAME_SYMB_ANCHORAGE);
  i046->addVariant(StrId::ARRAY_NAME_SYMB_COASTAL_LANDMARK);
  m04->add_item(static_cast<MenuItem*>(i046));

  MenuItemInt* i047 = new MenuItemInt(StrId::MENU_047, 1, 4, 1);
  m04->add_item(static_cast<MenuItem*>(i047));
  routeSaverItem = i047;


  // --------------------------
  MenuItemMenu* m05 = new MenuItemMenu(StrId::MENU_05, m0);
  m0->add_item(m05);

  MenuItemList* i050 = new MenuItemList(StrId::MENU_050, 0);
  i050->addVariant(StrId::ARRAY_NAME_SIGN_UNINDENT);
  i050->addVariant(StrId::ARRAY_NAME_SIGN_FRIENDLY);
  i050->addVariant(StrId::ARRAY_NAME_SIGN_ENEMY);
  m05->add_item(static_cast<MenuItem*>(i050));

  MenuItemList* i051 = new MenuItemList(StrId::MENU_051, 0);
  i051->addVariant(StrId::ARRAY_NAME_RECOG_OFF);
  i051->addVariant(StrId::ARRAY_NAME_RECOG_ROUND);
  i051->addVariant(StrId::ARRAY_NAME_RECOG_SECT);
  m05->add_item(static_cast<MenuItem*>(i051));

  MenuItemList* i052 = new MenuItemList(StrId::MENU_052, 0);
  i052->addVariant(StrId::ARRAY_OFFON_OFF);
  i052->addVariant(StrId::ARRAY_OFFON_ON);
  m05->add_item(static_cast<MenuItem*>(i052));

  // --------------------------
  _main_menu = m0;
}

MenuEngine::~MenuEngine() {
  glDeleteBuffers(ATTR_COUNT, _vbo_ids);
  delete _main_menu;
  delete _cnfg_menu;
}

void MenuEngine::onStateChanged(WidgetState state) {
  switch (state) {
  case WidgetState::MAIN_MENU:
      _menu = _main_menu;
      break;
  case WidgetState::CONFIG_MENU:
      _menu = _cnfg_menu;
      break;
  case WidgetState::DEFAULT:
  case WidgetState::MAGNIFIER:
      _selection_active = false;
      _menu = nullptr;
      break;
  case WidgetState::ROUTE_EDITION:
      break;
  }

  _selected_line = 1;
  _need_update = true;
  _last_action_time = QDateTime::currentDateTime();
}

void MenuEngine::onAnalogZeroChanged(int val) {
  analogZeroItem->setValue(val);
}


void MenuEngine::onKeyPressed(QKeyEvent* event) {
  switch(event->key()) {
  // Выбор цели
  case Qt::Key_Up:
    onUp();
    break;

  // ЛИД / ЛОД
  case Qt::Key_Down:
    onDown();
    break;

  // Захват
  case Qt::Key_Return:
  case Qt::Key_Enter:
    onEnter();
    break;

  //Сброс
  case Qt::Key_Escape:
    onBack();
    break;

  }
}

void MenuEngine::onUp() {
  if (_menu == nullptr)
    return;

  if (!_selection_active) {
    if (_selected_line > 1)
      _selected_line--;
  } else {
    _menu->item(_selected_line - 1)->up();
  }

  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::onDown() {
  if (_menu == nullptr)
    return;

  if (!_selection_active) {
    if (_selected_line < _menu->item_count())
      _selected_line++;
  } else {
    _menu->item(_selected_line - 1)->down();
  }

  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::onEnter() {
  if ( (_menu->item(_selected_line-1)->locked()
        || !_menu->item(_selected_line-1)->enabled() )
       && !_selection_active)
    return;

  if (_menu->item(_selected_line - 1)->type() == MenuItem::Type::MENU) {
    _menu = dynamic_cast<MenuItemMenu*>(_menu->item(_selected_line - 1));
    _selected_line = 1;
  } else {
    _selection_active = !_selection_active;

    if (_menu->item(_selected_line - 1) == routeEditItem) {
      if (_selection_active)
        emit startRouteEdit();
      else
        emit finishRouteEdit();
    }

    if (!_selection_active) {
      if (_menu->item(_selected_line - 1) == routeLoaderItem)
        emit loadRoute(routeLoaderItem->intValue()-1);

      if (_menu->item(_selected_line - 1) == routeSaverItem)
        emit saveRoute(routeSaverItem->intValue()-1);

    }
  }

  _last_action_time = QDateTime::currentDateTime();
  _need_update = true;
}

void MenuEngine::onBack() {
  if (!_selection_active && _menu->parent() != nullptr) {
     int _new_selection = 1;
     for (int i = 0; i < _menu->parent()->item_count(); i++)
       if (_menu->parent()->item(i) == _menu)
         _new_selection = i+1;

     _selected_line = _new_selection;
     _menu = _menu->parent();

     _last_action_time = QDateTime::currentDateTime();
     _need_update = true;
  }
}


void MenuEngine::resizeTexture(const Layout& layout) {
  TextureLayerBase::resize(layout.menu.geometry);

  _font_tag = layout.menu.font;
  _need_update = true;
}

void MenuEngine::clearTexture() {
  TextureLayerBase::clear(1.f, 1.f, 1.f, 0.f, 0.f);
}


void MenuEngine::paint(const State& state, const Layout& layout) {
  if (!_need_update)
    return;

  glViewport(0, 0, width(), height());

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());

  glClearColor(MENU_BACKGRD_COLOR.redF(), MENU_BACKGRD_COLOR.greenF(), MENU_BACKGRD_COLOR.blueF(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT);


  glUseProgram(progId());

  glUniformMatrix4fv(unifLoc(UNIF_MVP), 1, GL_FALSE, projection().data());

  // Border
  drawRect(QRect(QPoint(0, 0), QSize(width(), 1)), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(0, 0), QSize(1, height())), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(0, height()-1), QSize(width(), 1)), MENU_BORDER_COLOR);
  drawRect(QRect(QPoint(width()-1, 0), QSize(1, height())), MENU_BORDER_COLOR);

  if (_menu != nullptr) {
    QSize font_size = _fonts->fontSize(_font_tag);

    // Header separator
    drawRect(QRect(QPoint(0, font_size.height() + 6), QSize(width(), 1)), MENU_BORDER_COLOR);
    // Footer separator
    drawRect(QRect(QPoint(0, height() - font_size.height() - 6), QSize(width(), 1)), MENU_BORDER_COLOR);

    // Header
    drawText(_menu->name(state.lang), 0, ALIGN_CENTER, MENU_TEXT_STATIC_COLOR);

    // Menu
    for (int i = 0; i < _menu->item_count(); i++) {
//      if ((_menu->item(i) == routeLoaderItem || _menu->item(i) == routeSaverItem) && _routeEngine != nullptr) {
//        if (_routeEngine->isIndexUsed(static_cast<MenuItemInt*>(_menu->item(i))->intValue())) {
//          drawText(_menu->item(i)->name(_lang), i+1, ALIGN_LEFT, MENU_TEXT_STATIC_COLOR);
//          drawText(_menu->item(i)->value(_lang), i+1, ALIGN_RIGHT, MENU_TEXT_DYNAMIC_COLOR);
//        } else {
//          drawText(_menu->item(i)->name(_lang), i+1, ALIGN_LEFT, MENU_TEXT_STATIC_COLOR);
//          drawText(_menu->item(i)->value(_lang), i+1, ALIGN_RIGHT, MENU_DISABLED_ITEM_COLOR);
//        }

//        continue;
//      }

      if (_menu->item(i)->locked()) {
        drawText(_menu->item(i)->name(state.lang), i+1, ALIGN_LEFT, MENU_LOCKED_ITEM_COLOR);
        drawText(_menu->item(i)->value(state.lang), i+1, ALIGN_RIGHT, MENU_LOCKED_ITEM_COLOR);
      } else if (_menu->item(i)->enabled()) {
        drawText(_menu->item(i)->name(state.lang), i+1, ALIGN_LEFT, MENU_TEXT_STATIC_COLOR);
        drawText(_menu->item(i)->value(state.lang), i+1, ALIGN_RIGHT, MENU_TEXT_DYNAMIC_COLOR);
      } else {
        drawText(_menu->item(i)->name(state.lang), i+1, ALIGN_LEFT, MENU_LOCKED_ITEM_COLOR);
        drawText(_menu->item(i)->value(state.lang), i+1, ALIGN_RIGHT, MENU_DISABLED_ITEM_COLOR);
      }
    }

    drawBar();
    drawSelection();
  }

  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _need_update = false;
}

void MenuEngine::drawSelection() {
  QSize font_size = _fonts->fontSize(_font_tag);

  QColor col;
  if (_selection_active)
    col = MENU_TEXT_DYNAMIC_COLOR;
  else
    col = MENU_TEXT_STATIC_COLOR;

  QPoint anchor = QPoint(2, 2+_selected_line*(6+font_size.height()));
  QSize  size = QSize(width() - 4, font_size.height() + 4);

  drawRect(QRect(anchor, QSize(size.width(), 1)), col);
  drawRect(QRect(anchor, QSize(1, size.height())), col);
  drawRect(QRect(anchor + QPoint(0, size.height() - 1), QSize(size.width(), 1)), col);
  drawRect(QRect(anchor + QPoint(size.width()-1, 0), QSize(1, size.height())), col);
}

void MenuEngine::drawBar() {
  if (_menu == nullptr)
    return;

  int bar_width = 0;

  MenuItem* currItem = _menu->item(_selected_line - 1);

  MenuItemInt* intItem = dynamic_cast<MenuItemInt*>(currItem);
  if (intItem != nullptr) {
    double val = intItem->intValue();
    double min_val = intItem->minValue();
    double max_val = intItem->maxValue();
    bar_width = static_cast<int>((width() - 2) * (val - min_val) / (max_val - min_val));
  }

  MenuItemReal* fltItem = dynamic_cast<MenuItemReal*>(currItem);
  if (fltItem != nullptr) {
    double val = fltItem->fltValue();
    double min_val = fltItem->minValue();
    double max_val = fltItem->maxValue();
    bar_width = static_cast<int>((width() - 2) * (val - min_val) / (max_val - min_val));
  }

  drawRect(QRect(QPoint(1, height()-17), QSize(bar_width, 14)), MENU_BORDER_COLOR);
}

void MenuEngine::drawRect(const QRect& rect, const QColor& col) {
  std::vector<GLfloat> pos;

  pos.push_back(rect.x());
  pos.push_back(rect.y());
  pos.push_back(rect.x());
  pos.push_back(rect.y() + rect.height());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y() + rect.height());

  glUniform2f(unifLoc(UNIF_SIZE), 0.0, 0.0);
  glUniform4f(unifLoc(UNIF_COLOR), col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_POSITION), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_POSITION));

  glVertexAttrib1f(attrLoc(ATTR_ORDER), 0.0);
  glDisableVertexAttribArray(attrLoc(ATTR_ORDER));

  glVertexAttrib1f(attrLoc(ATTR_CHAR_VAL), 0.0);
  glDisableVertexAttribArray(attrLoc(ATTR_CHAR_VAL));

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void MenuEngine::drawText(const QByteArray& text, int line, TextAllignement align, const QColor& col) {
  GLuint tex_id = _fonts->texture(_font_tag);
  QSize font_size = _fonts->fontSize(_font_tag);

  std::vector<GLfloat> pos;
  std::vector<GLfloat> ord, chars;

  QPoint anchor = QPoint(0, 4 + (font_size.height() + 6) * line);

  switch (align) {
    case ALIGN_CENTER:
      anchor.setX(width()/2 - text.size()*font_size.width()/2);
      break;
    case ALIGN_LEFT:
      anchor.setX(4);
      break;
    case ALIGN_RIGHT:
      anchor.setX(width() - 4 - text.size()*font_size.width());
      break;
  }

  // From text to vertex data
  for (int i = 0; i < text.size(); i++) {
    for (int j = 0; j < 4; j++) {
      QPoint lefttop = anchor + QPoint(i * font_size.width(), 0);
      pos.push_back(lefttop.x());
      pos.push_back(lefttop.y());
      ord.push_back(j);
      chars.push_back(text[i]);
    }
  }


  glUniform2f(unifLoc(UNIF_SIZE), font_size.width(), font_size.height());
  glUniform4f(unifLoc(UNIF_COLOR), col.redF(), col.greenF(), col.blueF(), 1.0);


  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_POSITION), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_POSITION));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, ord.size()*sizeof(GLfloat), ord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_ORDER), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, chars.size()*sizeof(GLfloat), chars.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_CHAR_VAL), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_CHAR_VAL));


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, ord.size());

  glBindTexture(GL_TEXTURE_2D, 0);
}
