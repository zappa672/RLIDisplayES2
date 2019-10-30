#include "strings.h"

#include <QTextCodec>
#include <QTextEncoder>
#include <QTextDecoder>

using namespace RLI;

void Strings::addString(StrId name, QVector<QString> vals) {
  QVector<QByteArray> enc_vals(static_cast<int>(Lang::COUNT));

  for (int i = 0; i < static_cast<int>(Lang::COUNT); i++)
    enc_vals.insert(i, _encoder->fromUnicode(vals[i]));

  _strings.insert(name, enc_vals);
}

Strings::Strings() {
  _encoder = QTextCodec::codecForName("cp866")->makeEncoder();

  addString( StrId::BLANK, {"", ""} );

  // для главного меню
  addString( StrId::MENU_0, { "MAIN MENU", "ГЛАВНОЕ МЕНЮ" } );

  addString( StrId::MENU_00, { "BRIGHTNESS", "ЯРКОСТЬ" } );
  addString( StrId::MENU_01, { "AUTO PLOTTING", "АВТОПРОКЛАДКА" } );
  addString( StrId::MENU_02, { "SETTINGS", "УСТАНОВКИ" } );
  addString( StrId::MENU_03, { "RADAR TEST", "КОНТРОЛЬ" } );
  addString( StrId::MENU_04, { "MAP", "КАРТА" } );
  addString( StrId::MENU_05, { "RECOGNITION", "ОПОЗНАВАНИЕ" } );

  addString( StrId::MENU_000, { "RADAR PICTURE", "РЛ ВИДЕО " } );
  addString( StrId::MENU_001, { "MEASURES", "ИЗМЕРИТ СРЕДСТВА" } );
  addString( StrId::MENU_002, { "FCC", "МД" } );
  addString( StrId::MENU_003, { "PLOT SYMBOLS", "СИМВОЛЫ АРП" } );
  addString( StrId::MENU_004, { "TAILS", "СЛЕДЫ" } );
  addString( StrId::MENU_005, { "SCREEN PANEL", "ПАНЕЛЬ" } );
  addString( StrId::MENU_006, { "CONTROL PANEL", "ПУЛЬТ" } );
  addString( StrId::MENU_007, { "MAP", "КАРТА" } );
  addString( StrId::MENU_008, { "DAY/NIGHT", "ДЕНЬ/НОЧЬ" } );

  addString( StrId::MENU_010, { "CPA  LIMIT nm", "ОПАСН ДКС миль" } );
  addString( StrId::MENU_011, { "TCPA LIMITmin", "ОПАСН ВКС  мин" } );
  addString( StrId::MENU_012, { "VECTORS    min", "ВЕКТОРЫ    мин" } );
  addString( StrId::MENU_013, { "TAILS/PNT min", "СЛЕДЫ/ТЧК  мин" } );
  addString( StrId::MENU_014, { "NUMBER TARGET", "НОМЕРА ЦЕЛЕЙ" } );
  addString( StrId::MENU_015, { "AZ DELETE", "СБРОС ЗОНЫ" } );
  addString( StrId::MENU_016, { "AZ SETTING", "УСТАН ЗОНЫ" } );
  addString( StrId::MENU_017, { "TRUE VECTOR", "ЛИД" } );

  addString( StrId::MENU_020, { "TUNE", "РПЧ" } );
  addString( StrId::MENU_021, { "CHANGE VRM", "СМЕНА ВД" } );
  addString( StrId::MENU_022, { "SPEED DEVICE", "ДАТЧИК СКОР" } );
  addString( StrId::MENU_023, { "MAN SPD kts ", "СК  РУЧ  уз " } );
  addString( StrId::MENU_024, { "STAB SYSTEM", "ДАТЧИК СТАБ" } );
  addString( StrId::MENU_025, { "TIMER min", "ТАЙМЕР мин" } );
  addString( StrId::MENU_026, { "LANGUAGE", "ЯЗЫК" } );
  addString( StrId::MENU_027, { "GYRO ALIGN", "СОГЛАС ГИРО" } );
  addString( StrId::MENU_028, { "DANGER DPT m","ОПАС ГЛУБ м" } );
  addString( StrId::MENU_029, { "ANCHORAGE m", "ЯКОРНАЯ СТ м" } );
  addString( StrId::MENU_02A, { "STAB NOISE", "ШУМ СТАБ" } );

  addString( StrId::MENU_030, { "STROBE AT", "СТРОБ АС" } );
  addString( StrId::MENU_031, { "TARGETS SIMUL", "ИМИТАЦИЯ ЦЕЛИ" } );
  addString( StrId::MENU_032, { "PERFORM MON", "ЭП" } );
  addString( StrId::MENU_033, { "PEAK POWER", "КИМ" } );
  addString( StrId::MENU_034, { "MAGNETRON", "ТОК МАГНЕТР" } );
  addString( StrId::MENU_035, { "DISP TRIG", "ЗАПУСК И" } );
  addString( StrId::MENU_036, { "RADAR VIDEO", "РЛ ВИДЕО" } );
  addString( StrId::MENU_037, { "AZ PULSE", "КУА" } );
  addString( StrId::MENU_038, { "HEAD PULSE", "ОК" } );

  addString( StrId::MENU_040, { "CALL ROUTE №", "ВЫЗОВ КАРТЫ №" } );
  addString( StrId::MENU_041, { "TAILS/PNT", "СЛЕДЫ/ТЧК" } );
  addString( StrId::MENU_042, { "-------------------", "-------------------" } );
  addString( StrId::MENU_043, { "FARWATER m", "ФАРВАТЕР м" } );
  addString( StrId::MENU_044, { "LINE OF A ROUTE", "ЛИНИЯ МАРШРУТА" } );
  addString( StrId::MENU_045, { "POINT NUMBER", "НОМЕР ТОЧКИ" } );
  addString( StrId::MENU_046, { "SYMBOLS", "СИМВОЛЫ" } );
  addString( StrId::MENU_047, { "RECORDING MAP №", "ЗАПИСЬ КАРТЫ №" } );

  addString( StrId::MENU_050, { "SIGN", "ПРИЗНАК" } );
  addString( StrId::MENU_051, { "REQUEST", "ЗАПРОС" } );
  addString( StrId::MENU_052, { "ABORT EMIS", "ЗАПРЕТ ИЗЛУЧ" } );


  // для меню конфигурация
  addString( StrId::MENU_1, { "CONFIG", "КОНФИГУРАЦИЯ" } );

  addString( StrId::MENU_10, { "TUNINGS", "НАСТРОЙКИ" } );
  addString( StrId::MENU_11, { "TUNINGS PIKO", "НАСТРОЙКИ ПИКО" } );
  addString( StrId::MENU_12, { "NMEA ACCEPT", "ПРИЕМ NMEA" } );
  addString( StrId::MENU_13, { "NMEA TRANSM", "ПЕРЕДАЧА NMEA" } );

  addString( StrId::MENU_100, { "LOG SIGNAL", "СИГНАЛ ЛАГА" } );
  addString( StrId::MENU_101, { "RANGE CCN", "КОР ДАЛЬН" } );
  addString( StrId::MENU_102, { "BEARING CCN", "КОР НАПР" } );
  addString( StrId::MENU_103, { "BEGIN TR OFF", "НАЧ ОТКЛ ПП" } );
  addString( StrId::MENU_104, { "END TR OFF", "КОН ОТКЛ ПП" } );
  addString( StrId::MENU_105, { "BAND    ", "ДИАПАЗОН" } );
  addString( StrId::MENU_106, { "GRAPH AFC", "ГРАФИК АПЧ" } );
  addString( StrId::MENU_107, { "MASTER BRIGHTN", "ОБЩАЯ ЯРКОСТЬ" } );

  addString( StrId::MENU_110, { "MAX VIDEO", "ДИН ДИАП ВС" } );
  addString( StrId::MENU_111, { "TOP MARGIN", "ОГРАН ВЕРХОВ" } );
  addString( StrId::MENU_112, { "ANALOG ZERO", "АНАЛОГ 0 ВС" } );
  addString( StrId::MENU_113, { "NOISE MARGIN", "ОГРАН ШУМОВ" } );
  addString( StrId::MENU_114, { "TRESH AT", "ПОРОГ АС" } );
  addString( StrId::MENU_115, { "TRESH AT", "ПОРОГ СЛЕДОВ" } );
  addString( StrId::MENU_116, { "THRESH ZONE", "ПОРОГ ЗОНЫ" } );
  addString( StrId::MENU_117, { "THRESH ACCUM", "ПОР ЯРК НАКОП" } );
  addString( StrId::MENU_118, { "DELTA ARPA 6", "ДЕЛЬТА САРП 6" } );
  addString( StrId::MENU_119, { "DELTA ZONE 6", "ДЕЛЬТА ЗОНЫ 6" } );
  addString( StrId::MENU_11A, { "DELTA ZONE 12", "ДЕЛЬТА ЗОНЫ 12" } );

  addString( StrId::MENU_120, { "GGA ", "GGA " } );
  addString( StrId::MENU_121, { "VTG ", "VTG " } );
  addString( StrId::MENU_122, { "ZDA ", "ZDA " } );
  addString( StrId::MENU_123, { "DPT ", "DPT " } );
  addString( StrId::MENU_124, { "DBT ", "DBT " } );
  addString( StrId::MENU_125, { "VBW ", "VBW " } );
  addString( StrId::MENU_126, { "VHW ", "VHW " } );
  addString( StrId::MENU_127, { "RMC ", "RMC " } );

  addString( StrId::MENU_130, { "TTM", "TTM" } );
  addString( StrId::MENU_131, { "VHW", "VHW" } );
  addString( StrId::MENU_132, { "RSD", "RSD" } );
  addString( StrId::MENU_133, { "BAUDRATE", "СК ОБМЕНА" } );

  addString( StrId::EMISSION,    { "EMISSION",	"ИЗЛУЧЕНИЕ" } );
  addString( StrId::DEGREE_SIGN, { "°   ",	"°   " } );
  addString( StrId::GAIN,        { "GAIN", "УСИЛЕНИЕ" } );
  addString( StrId::RAIN,        { "RAIN", "ДОЖДЬ" } );
  addString( StrId::RAIN_1,      { "RAIN 1", "ДОЖДЬ 1" } );
  addString( StrId::WAVE,        { " SEA ", "ВОЛНЫ" } );
  addString( StrId::AFC,         { "AFC", "АПЧ" } );
  addString( StrId::TUNE,        { "TUNE", "РПЧ" } );
  addString( StrId::PP12p,       { "_П___", "_П___" } );
  addString( StrId::MODE,        { "MODE", "РЕЖИМЫ" } );
  addString( StrId::HEAD,        { "HEAD", "КУРС" } );
  addString( StrId::NORTH,       { "NORTH", "СЕВЕР" } );
  addString( StrId::COURSE,      { "COURSE", "КУРС СТ" } );
  addString( StrId::RM,          { "RM", "ОД" } );
  addString( StrId::TM,          { "TM", "ИД" } );
  addString( StrId::GROUND,      { "GROUND", "ГРУНТ" } );
  addString( StrId::WATER,       { "WATER", "ВОДА" } );
  addString( StrId::EBL,         { "EBL", "ВН" } );
  addString( StrId::B,           { "B ", "П " } );
  addString( StrId::TAILS,       { "TAILS", "СЛЕДЫ" } );
  addString( StrId::POINTS,      { "POINTS", "ТОЧКИ" } );
  addString( StrId::MIN,         { "MIN ", "МИН " } );
  addString( StrId::VRM,         { "VRM", "ВД" } );
  addString( StrId::BLINK,       { "  ", "  " } );
  addString( StrId::NM,          { "NM  ", "МИЛЬ" } );
  addString( StrId::KM,          { "KM  ", "КМ  " } );
  addString( StrId::METER,       { "M   ", "М   " } );
  addString( StrId::GYRO_HDG,    { "GYRO HDG ", "КУРС ГИРО" } );
  addString( StrId::GYRO_OFF,    { "GYRO OFF ", "КУРС ОТКЛ" } );
  addString( StrId::LOG_SPD_S,   { "LOG SPD(S)", "СК ЛАГ(С) " } );
  addString( StrId::LOG_SPD_W,   { "LOG SPD(W)", "СК ЛАГ(В) " } );
  addString( StrId::MAN_SPD,     { "MAN SPD   ", "СК РУЧ    " } );
  addString( StrId::KTS,         { "KTS ", "УЗ  " } );
  addString( StrId::KM_H,        { "KM/H", "КМ/Ч" } );
  addString( StrId::LAT,         { "LAT    ", "ШИРОТА " } );
  addString( StrId::LON,         { "LON    ", "ДОЛГОТА" } );
  addString( StrId::ACT_COURSE,  { "ACT COURSE  ", "КУРС АОЦ  " } );
  addString( StrId::GPS_COURSE,  { "GPS COURSE  ", "КУРС СНС  " } );
  addString( StrId::DLG_COURSE,  { "DLG COURSE  ", "КУРС ДЛГ  " } );
  addString( StrId::ACT_SPEED,   { "ACT SPEED", "СКОР АОЦ " } );
  addString( StrId::GPS_SPEED,   { "GPS SPEED", "СКОР СНС " } );
  addString( StrId::DLG_SPEED,   { "DLG SPEED", "СКОР ДЛГ " } );
  addString( StrId::VECTOR,      { "VECTOR", "ВЕКТОР" } );
  addString( StrId::DANGER_TRG,  { "DANGER TRG", "ОПАСНАЯ ЦЕЛЬ" } );
  addString( StrId::CPA_LIMIT,   { "CPA LIMIT  ", "ОПАСНАЯ ДКС" } );
  addString( StrId::TCPA_LIMIT,  { "TCPA LIMIT ", "ОПАСНОЕ ВКС" } );
  addString( StrId::TRG_N_ALL,   { "TRG  №    ALL", "ЦЕЛЬ №    ВСЕГО" } );
  addString( StrId::TRG_ALL,     { "TRG       ALL", "ЦЕЛЬ      ВСЕГО" } );
  addString( StrId::BEARING,     { "BEARING  ", "ПЕЛЕНГ   " } );
  addString( StrId::RANGE,       { "RANGE    ", "ДАЛЬНОСТЬ" } );
  addString( StrId::COURSE_W,    { "COURSE(W)", "КУРС  (В)" } );
  addString( StrId::SPEED_W,     { "SPEED (W)", "СКОР  (В)" } );
  addString( StrId::COURSE_G,    { "COURSE(G)", "КУРС  (Г)" } );
  addString( StrId::SPEED_G,     { "SPEED (G)", "СКОР  (Г)" } );
  addString( StrId::CPA,         { "CPA", "ДКС" } );
  addString( StrId::TCPA,        { "TCPA", "ВКС " } );
  addString( StrId::DCC,         { "DCC", "ДПК" } );
  addString( StrId::TCC,         { "TCC", "ВПК" } );
  addString( StrId::CURSOR,      { "CURSOR", "КУРСОР" } );
  addString( StrId::HB,          { "HB       ", "КУРС.УГОЛ" } );
  addString( StrId::OFF,         { "OFF", "ОТКЛ" } );
  addString( StrId::ON,          { "ON", " ВКЛ" } );
  addString( StrId::AUTO_A_C,    { "AUTO A/C", "АПЗ" } );
  addString( StrId::X_BAND,      { "X-BAND", "Х-ДИАП" } );
  addString( StrId::S_BAND,      { "S-BAND", "S-ДИАП" } );
  addString( StrId::K_BAND,      { "K-BAND", "K-ДИАП" } );

  addString( StrId::LOD,         { "???", "ЛОД" } );
  addString( StrId::CU,          { "??", "КУ" } );
  addString( StrId::GRAD_LB,     { "°LB",	"°ЛБ" } );
  addString( StrId::GRAD_RB,     { "°RB",	"°ПБ" } );

  addString( StrId::TIME,        { "TIME", "ВРЕМЯ" } );
  addString( StrId::FPS,         { "FPS", "КАДР/С"});

  // для маршрута
  addString( StrId::GO_TO_RP,    { "GO TO RP", "ИДЕМ НА МТ" } );
  addString( StrId::PARAM_OF_A_ROUTE,   { "PARAM OF A ROUTE", "ПАРАМЕТРЫ МАРШРУТА" } );
  addString( StrId::FAIRWAY_m,   { "FAIRWAY m", "ФАРВАТЕР м" } );
  addString( StrId::DEFLECT,     { "DEFLECT   ", "ОТКЛОНЕН  " } );
  addString( StrId::RANGE_RP,    { "RANGE RP", "ДАЛЬН МТ" } );
  addString( StrId::TIME_RP,     { "TIME  RP", "ВРЕМЯ МТ" } );
  addString( StrId::TA_ON_RP,    { "TA ON RP", "ПУ НА МТ" } );
  addString( StrId::NEW_COURSE,  { "NEW COURSE", "НОВЫЙ КУРС" } );
  addString( StrId::DEPTH,       { "DEPTH", "ГЛУБИНА" } );
  addString( StrId::METRS,       { "METRS", "МЕТРЫ" } );

  // выбор день/ночь
  addString( StrId::ARRAY_DAY_NIGHT, { "NIGHT", "НОЧЬ" } );
  addString( StrId::ARRAY_DAY_DAY,   { "DAY", " ДЕНЬ" } );

  // значения следов/точек для меню
  addString( StrId::ARRAY_TRACK_1,   { "1", "1" } );
  addString( StrId::ARRAY_TRACK_2,   { "2", "2" } );
  addString( StrId::ARRAY_TRACK_3,   { "3", "3" } );
  addString( StrId::ARRAY_TRACK_6,   { "6", "6" } );
  addString( StrId::ARRAY_TRACK_12,  { "12", "12" } );

  // выбор ОТКЛ/ВКЛ
  addString( StrId::ARRAY_OFFON_OFF, { "OFF", "ОТКЛ" } );
  addString( StrId::ARRAY_OFFON_ON,  { "ON", "ВКЛ" } );

  // выбор ДА/НЕТ
  addString( StrId::ARRAY_YESNO_YES, { "YES", "ДА" } );
  addString( StrId::ARRAY_YESNO_NO,  { "NO", "НЕТ" } );

  // выбор ЛИД для АС
  addString( StrId::ARRAY_TVEC_AP_WATER, { "WATER", "ВОДА" } );
  addString( StrId::ARRAY_TVEC_AP_GRND,  { "GRND", "ГРУНТ" } );

  // значения типа скорости для меню
  addString( StrId::ARRAY_SPEED_MAN,   { "MAN", "РУЧ" } );
  addString( StrId::ARRAY_SPEED_LOG,   { "LOG", "ЛАГ" } );

  // выбор языка системы
  addString( StrId::ARRAY_LANG_ENGL,   { "ENGL", "АНГЛ" } );
  addString( StrId::ARRAY_LANG_RUS,    { "RUS", "РУС" } );

  // выбор датчика стабилизации
  addString( StrId::ARRAY_DEV_STAB_ATER,  { "ATER", "АОЦ" } );
  addString( StrId::ARRAY_DEV_STAB_GPS,   { "GPS", "СНС" } );
  addString( StrId::ARRAY_DEV_STAB_DLG,   { "DLG", "ДЛГ" } );
  addString( StrId::ARRAY_DEV_STAB_L_G_W, { "L-G(W)", "ГК-Л(В)" } );

  // выбор единиц измерения ВД
  addString( StrId::ARRAY_VD_KM, { "KM", "КМ" } );
  addString( StrId::ARRAY_VD_NM, { "NM", "МИЛИ" } );

  // символы маршрута
  addString( StrId::ARRAY_NAME_SYMB_BUOY,              { "╚", "╚" } ); // буй                  0xc8 в cp866
  addString( StrId::ARRAY_NAME_SYMB_MILESTONE,         { "╔", "╔" } ); // веха                 0xc9 в cp866
  addString( StrId::ARRAY_NAME_SYMB_UNDERWATER_DAMAGE, { "╩", "╩" } ); // подводная опасность  0xca в cp866
  addString( StrId::ARRAY_NAME_SYMB_ANCHORAGE,         { "╦", "╦" } ); // якорная стоянка      0xcb в cp866
  addString( StrId::ARRAY_NAME_SYMB_COASTAL_LANDMARK,  { "╠", "╠" } ); // береговой ориентир   0xcc в cp866

  // опознавание
  addString( StrId::ARRAY_NAME_RECOG_OFF,    { "OFF", "НЕТ" } );
  addString( StrId::ARRAY_NAME_RECOG_ROUND,  { "ROUND", "КРУГ" } );
  addString( StrId::ARRAY_NAME_RECOG_SECT,   { "SECT", "СЕКТ" } );

  addString( StrId::ARRAY_NAME_SIGN_UNINDENT,  { "UNINDENT", "НЕТ" } );
  addString( StrId::ARRAY_NAME_SIGN_FRIENDLY,  { "FRIENDLY", "СВОЙ" } );
  addString( StrId::ARRAY_NAME_SIGN_ENEMY,     { "ENEMY", "ЧУЖОЙ" } );

  // выбор типа лага
  addString( StrId::ARRAY_LOG_SIGNAL_200_MINUS,  { "200-", "200-" } );
  addString( StrId::ARRAY_LOG_SIGNAL_500_MINUS,  { "500-", "500-" } );
  addString( StrId::ARRAY_LOG_SIGNAL_COD_MINUS,  { "COD-", "КОД-" } );
  addString( StrId::ARRAY_LOG_SIGNAL_NMEA,       { "NMEA", "NMEA" } );
  addString( StrId::ARRAY_LOG_SIGNAL_COD_PLUS,   { "COD+", "КОД+" } );
  addString( StrId::ARRAY_LOG_SIGNAL_500_PLUS,   { "500+", "500+" } );
  addString( StrId::ARRAY_LOG_SIGNAL_200_PLUS,   { "200+", "200+" } );
  addString( StrId::ARRAY_LOG_SIGNAL_GPS,        { "GPS", "СНС" } );

  // выбор диапазона
  addString( StrId::ARRAY_BAND_X, { "X", "X" } );
  addString( StrId::ARRAY_BAND_S, { "S", "S" } );
  addString( StrId::ARRAY_BAND_K, { "K", "K" } );
}

Strings::~Strings(void) {
  for (auto _string_vec : _strings)
    _string_vec.clear();

  _strings.clear();
}
