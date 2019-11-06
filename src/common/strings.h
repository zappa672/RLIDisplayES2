#ifndef Strings_H
#define Strings_H

#include <QMap>
#include <QVector>
#include <QByteArray>
#include <QTextEncoder>

#include <QObject>

namespace RLI {

  enum class Lang : int {
    ENGLISH  = 0
  , RUSSIAN  = 1
  , COUNT    = 2 };

  enum class StrId;  

  class Strings {
  public:    
    static Strings& instance() {            
      static Strings instance;
      return instance;
    }

    const QByteArray string(Lang lang, StrId name) const { return _strings[name][static_cast<int>(lang)]; }
    QTextEncoder* encoder() const { return _encoder; }

  private:
    Strings(void);
    ~Strings(void);

    // Singleton
    Strings(Strings const&) = delete;
    Strings& operator= (Strings const&) = delete;
    Strings(Strings const&&) = delete;
    Strings& operator= (Strings const&&) = delete;

    QTextEncoder* _encoder;
    void addString(StrId name, QVector<QString> vals);

    QMap<StrId, QVector<QByteArray>> _strings;
  };

  enum class StrId {
      NONE
    , BLANK

    , MENU_0

    , MENU_00
    , MENU_01
    , MENU_02
    , MENU_03
    , MENU_04
    , MENU_05

    , MENU_000
    , MENU_001
    , MENU_002
    , MENU_003
    , MENU_004
    , MENU_005
    , MENU_006
    , MENU_007
    , MENU_008

    , MENU_010
    , MENU_011
    , MENU_012
    , MENU_013
    , MENU_014
    , MENU_015
    , MENU_016
    , MENU_017

    , MENU_020
    , MENU_021
    , MENU_022
    , MENU_023
    , MENU_024
    , MENU_025
    , MENU_026
    , MENU_027
    , MENU_028
    , MENU_029
    , MENU_02A

    , MENU_030
    , MENU_031
    , MENU_032
    , MENU_033
    , MENU_034
    , MENU_035
    , MENU_036
    , MENU_037
    , MENU_038

    , MENU_040
    , MENU_041
    , MENU_042
    , MENU_043
    , MENU_044
    , MENU_045
    , MENU_046
    , MENU_047

    , MENU_050
    , MENU_051
    , MENU_052

    , MENU_1

    , MENU_10
    , MENU_11
    , MENU_12
    , MENU_13

    , MENU_100
    , MENU_101
    , MENU_102
    , MENU_103
    , MENU_104
    , MENU_105
    , MENU_106
    , MENU_107

    , MENU_110
    , MENU_111
    , MENU_112
    , MENU_113
    , MENU_114
    , MENU_115
    , MENU_116
    , MENU_117
    , MENU_118
    , MENU_119
    , MENU_11A

    , MENU_120
    , MENU_121
    , MENU_122
    , MENU_123
    , MENU_124
    , MENU_125
    , MENU_126
    , MENU_127

    , MENU_130
    , MENU_131
    , MENU_132
    , MENU_133

    , EMISSION
    , DEGREE_SIGN
    , GAIN
    , RAIN
    , RAIN_1
    , WAVE
    , AFC
    , TUNE
    , PP12p
    , MODE
    , HEAD
    , NORTH
    , COURSE
    , RM
    , TM
    , GROUND
    , WATER
    , EBL
    , B
    , TAILS
    , POINTS
    , MIN
    , VRM
    , BLINK
    , NM
    , KM
    , METER
    , GYRO_HDG
    , GYRO_OFF
    , LOG_SPD_S
    , LOG_SPD_W
    , MAN_SPD
    , KTS
    , KM_H
    , LAT
    , LON
    , ACT_COURSE
    , GPS_COURSE
    , DLG_COURSE
    , ACT_SPEED
    , GPS_SPEED
    , DLG_SPEED
    , VECTOR
    , DANGER_TRG
    , CPA_LIMIT
    , TCPA_LIMIT
    , TRG_N_ALL
    , TRG_ALL
    , BEARING
    , RANGE
    , COURSE_W
    , SPEED_W
    , COURSE_G
    , SPEED_G
    , CPA
    , TCPA
    , DCC
    , TCC
    , CURSOR
    , HB
    , OFF
    , ON
    , AUTO_A_C
    , X_BAND
    , S_BAND
    , K_BAND

    , LOD
    , CU
    , GRAD_LB
    , GRAD_RB

    , TIME
    , FPS

    // для маршрута
    , GO_TO_RP
    , PARAM_OF_A_ROUTE
    , FAIRWAY_m
    , DEFLECT
    , RANGE_RP
    , TIME_RP
    , TA_ON_RP
    , NEW_COURSE
    , DEPTH
    , METRS

    // выбор день/ночь
    , ARRAY_DAY_NIGHT
    , ARRAY_DAY_DAY

    // значения следов/точек для меню
    , ARRAY_TRACK_1
    , ARRAY_TRACK_2
    , ARRAY_TRACK_3
    , ARRAY_TRACK_6
    , ARRAY_TRACK_12

    // выбор ОТКЛ/ВКЛ
    , ARRAY_OFFON_OFF
    , ARRAY_OFFON_ON

    // выбор ДА/НЕТ
    , ARRAY_YESNO_YES
    , ARRAY_YESNO_NO

    // выбор ЛИД для АС
    , ARRAY_TVEC_AP_WATER
    , ARRAY_TVEC_AP_GRND

    // значения типа скорости для меню
    , ARRAY_SPEED_MAN
    , ARRAY_SPEED_LOG

    // выбор языка системы
    , ARRAY_LANG_ENGL
    , ARRAY_LANG_RUS

    // выбор датчика стабилизации
    , ARRAY_DEV_STAB_ATER
    , ARRAY_DEV_STAB_GPS
    , ARRAY_DEV_STAB_DLG
    , ARRAY_DEV_STAB_L_G_W

    // выбор единиц измерения ВД
    , ARRAY_VD_KM
    , ARRAY_VD_NM

    // символы маршрута
    , ARRAY_NAME_SYMB_BUOY
    , ARRAY_NAME_SYMB_MILESTONE
    , ARRAY_NAME_SYMB_UNDERWATER_DAMAGE
    , ARRAY_NAME_SYMB_ANCHORAGE
    , ARRAY_NAME_SYMB_COASTAL_LANDMARK

    // опознавание
    , ARRAY_NAME_RECOG_OFF
    , ARRAY_NAME_RECOG_ROUND
    , ARRAY_NAME_RECOG_SECT

    , ARRAY_NAME_SIGN_UNINDENT
    , ARRAY_NAME_SIGN_FRIENDLY
    , ARRAY_NAME_SIGN_ENEMY

    // выбор типа лага
    , ARRAY_LOG_SIGNAL_200_MINUS
    , ARRAY_LOG_SIGNAL_500_MINUS
    , ARRAY_LOG_SIGNAL_COD_MINUS
    , ARRAY_LOG_SIGNAL_NMEA
    , ARRAY_LOG_SIGNAL_COD_PLUS
    , ARRAY_LOG_SIGNAL_500_PLUS
    , ARRAY_LOG_SIGNAL_200_PLUS
    , ARRAY_LOG_SIGNAL_GPS

    // выбор диапазона
    , ARRAY_BAND_X
    , ARRAY_BAND_S
    , ARRAY_BAND_K
  };

}

#endif // Strings_H
