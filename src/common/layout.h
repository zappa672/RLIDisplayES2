#ifndef RLILAYOUT_H
#define RLILAYOUT_H

#include <QString>
#include <QColor>

#include <QPoint>
#include <QSize>
#include <QRect>

#include <QVector>
#include <QMap>
#include <QHash>

#include <QXmlStreamReader>

inline uint qHash(const QSize& s, uint seed)
{ return uint(s.width()) ^ uint(s.height()) ^ seed; }

inline uint qHash(const QSize& s)
{ return qHash(s, 0); }

namespace RLI {

  static constexpr size_t PNL_GAIN_TEXT_ID     = 0;
  static constexpr size_t PNL_WATER_TEXT_ID    = 0;
  static constexpr size_t PNL_RAIN_TEXT_ID     = 0;
  static constexpr size_t PNL_APCH_TEXT_ID     = 0;
  static constexpr size_t PNL_EMISSION_TEXT_ID = 0;


  static constexpr size_t PNL_LABEL5_TEXT_ID       = 0;
  static constexpr size_t PNL_BAND_TEXT_ID         = 0;
  static constexpr size_t PNL_ORIENTATION_TEXT_ID  = 0;
  static constexpr size_t PNL_LABEL2_TEXT_ID       = 0;
  static constexpr size_t PNL_LABEL3_TEXT_ID       = 0;
  static constexpr size_t PNL_LABEL4_TEXT_ID       = 0;

  static constexpr size_t PNL_FPS_LABEL_TEXT_ID  = 0;
  static constexpr size_t PNL_FPS_VALUE_TEXT_ID  = 1;


  static constexpr size_t PNL_SCALE_SCALE1_TEXT_ID = 0;
  static constexpr size_t PNL_SCALE_SLASH_TEXT_ID  = 1;
  static constexpr size_t PNL_SCALE_SCALE2_TEXT_ID = 2;
  static constexpr size_t PNL_SCALE_UNITS_TEXT_ID  = 3;

  static constexpr size_t PNL_VN_HEADER_TEXT_ID  = 0;
  static constexpr size_t PNL_VN_TBL_0_0_TEXT_ID = 1;
  static constexpr size_t PNL_VN_TBL_0_1_TEXT_ID = 2;
  static constexpr size_t PNL_VN_TBL_0_2_TEXT_ID = 3;
  static constexpr size_t PNL_VN_TBL_1_0_TEXT_ID = 4;
  static constexpr size_t PNL_VN_TBL_1_1_TEXT_ID = 5;
  static constexpr size_t PNL_VN_TBL_1_2_TEXT_ID = 6;

  static constexpr size_t PNL_VD_HEADER_TEXT_ID  = 0;
  static constexpr size_t PNL_VD_TBL_0_0_TEXT_ID = 1;
  static constexpr size_t PNL_VD_TBL_0_1_TEXT_ID = 2;

  static constexpr size_t PNL_COURSE_TBL_0_0_TEXT_ID = 0;
  static constexpr size_t PNL_COURSE_TBL_0_1_TEXT_ID = 1;
  static constexpr size_t PNL_COURSE_TBL_0_2_TEXT_ID = 2;
  static constexpr size_t PNL_COURSE_TBL_1_0_TEXT_ID = 3;
  static constexpr size_t PNL_COURSE_TBL_1_1_TEXT_ID = 4;
  static constexpr size_t PNL_COURSE_TBL_1_2_TEXT_ID = 5;


  static constexpr size_t PNL_POSITION_TBL_0_0_TEXT_ID = 0;
  static constexpr size_t PNL_POSITION_TBL_0_1_TEXT_ID = 1;
  static constexpr size_t PNL_POSITION_TBL_1_0_TEXT_ID = 2;
  static constexpr size_t PNL_POSITION_TBL_1_1_TEXT_ID = 3;

  //static constexpr size_t RLI_PNL_BLANK...

  static constexpr size_t PNL_CLOCK_LABEL_TEXT_ID  = 0;
  static constexpr size_t PNL_CLOCK_TIME_TEXT_ID   = 1;


  static constexpr size_t PNL_DANGER_LABEL_TEXT_ID = 0;

  static constexpr size_t PNL_TAILS_TBL_0_0_TEXT_ID  = 0;
  static constexpr size_t PNL_TAILS_TBL_0_1_TEXT_ID  = 1;
  static constexpr size_t PNL_TAILS_TBL_0_2_TEXT_ID  = 2;

  static constexpr size_t PNL_DANGER_INFO_TBL_0_0_TEXT_ID = 0;
  static constexpr size_t PNL_DANGER_INFO_TBL_0_1_TEXT_ID = 1;
  static constexpr size_t PNL_DANGER_INFO_TBL_0_2_TEXT_ID = 2;
  static constexpr size_t PNL_DANGER_INFO_TBL_1_0_TEXT_ID = 3;
  static constexpr size_t PNL_DANGER_INFO_TBL_1_1_TEXT_ID = 4;
  static constexpr size_t PNL_DANGER_INFO_TBL_1_2_TEXT_ID = 5;

  static constexpr size_t PNL_VECTOR_TBL_0_0_TEXT_ID = 0;
  static constexpr size_t PNL_VECTOR_TBL_0_1_TEXT_ID = 1;
  static constexpr size_t PNL_VECTOR_TBL_0_2_TEXT_ID = 2;

  static constexpr size_t PNL_TARGETS_HEADER_TEXT_ID   = 0;
  static constexpr size_t PNL_TARGETS_CURRENT_TEXT_ID  = 1;
  static constexpr size_t PNL_TARGETS_COUNT_TEXT_ID    = 2;
  static constexpr size_t PNL_TARGETS_TBL_0_0_TEXT_ID  = 3;
  static constexpr size_t PNL_TARGETS_TBL_0_1_TEXT_ID  = 4;
  static constexpr size_t PNL_TARGETS_TBL_0_2_TEXT_ID  = 5;
  static constexpr size_t PNL_TARGETS_TBL_1_0_TEXT_ID  = 6;
  static constexpr size_t PNL_TARGETS_TBL_1_1_TEXT_ID  = 7;
  static constexpr size_t PNL_TARGETS_TBL_1_2_TEXT_ID  = 8;
  static constexpr size_t PNL_TARGETS_TBL_2_0_TEXT_ID  = 9;
  static constexpr size_t PNL_TARGETS_TBL_2_1_TEXT_ID  = 10;
  static constexpr size_t PNL_TARGETS_TBL_2_2_TEXT_ID  = 11;
  static constexpr size_t PNL_TARGETS_TBL_3_0_TEXT_ID  = 12;
  static constexpr size_t PNL_TARGETS_TBL_3_1_TEXT_ID  = 13;
  static constexpr size_t PNL_TARGETS_TBL_3_2_TEXT_ID  = 14;
  static constexpr size_t PNL_TARGETS_TBL_4_0_TEXT_ID  = 15;
  static constexpr size_t PNL_TARGETS_TBL_4_1_TEXT_ID  = 16;
  static constexpr size_t PNL_TARGETS_TBL_4_2_TEXT_ID  = 17;
  static constexpr size_t PNL_TARGETS_TBL_5_0_TEXT_ID  = 18;
  static constexpr size_t PNL_TARGETS_TBL_5_1_TEXT_ID  = 19;
  static constexpr size_t PNL_TARGETS_TBL_5_2_TEXT_ID  = 20;
  static constexpr size_t PNL_TARGETS_TBL_6_0_TEXT_ID  = 21;
  static constexpr size_t PNL_TARGETS_TBL_6_1_TEXT_ID  = 22;
  static constexpr size_t PNL_TARGETS_TBL_6_2_TEXT_ID  = 23;
  static constexpr size_t PNL_TARGETS_TBL_7_0_TEXT_ID  = 24;
  static constexpr size_t PNL_TARGETS_TBL_7_1_TEXT_ID  = 25;
  static constexpr size_t PNL_TARGETS_TBL_7_2_TEXT_ID  = 26;

  static constexpr size_t PNL_CURSOR_HEADER_TEXT_ID  = 0;
  static constexpr size_t PNL_CURSOR_TBL_0_0_TEXT_ID = 1;
  static constexpr size_t PNL_CURSOR_TBL_0_1_TEXT_ID = 2;
  static constexpr size_t PNL_CURSOR_TBL_0_2_TEXT_ID = 3;
  static constexpr size_t PNL_CURSOR_TBL_1_0_TEXT_ID = 4;
  static constexpr size_t PNL_CURSOR_TBL_1_1_TEXT_ID = 5;
  static constexpr size_t PNL_CURSOR_TBL_1_2_TEXT_ID = 6;

  enum class PanelId : unsigned char
  { GAIN        = 0
  , WATER       = 1
  , RAIN        = 2
  , APCH        = 3
  , EMISSION    = 4

  , LABEL5      = 5
  , BAND        = 6
  , ORIENTATION = 7
  , LABEL2      = 8
  , LABEL3      = 9
  , LABEL4      = 10
  , FPS         = 11

  , SCALE       = 12
  , VN          = 13
  , VD          = 14
  , COURSE      = 15

  , POSITION    = 16
  , BLANK       = 17
  , CLOCK       = 18

  , DANGER      = 19
  , TAILS       = 20
  , DANGER_INFO = 21
  , VECTOR      = 22
  , TARGETS     = 23
  , CURSOR      = 24

  , COUNT       = 25 };


  enum class TextAllign { LEFT, RIGHT, CENTER };

  struct InfoRectLayout {
    QString name;
    QRect   rect;
    QColor  color;
  };

  struct InfoTextLayout {
    size_t text_id;
    QString name;
    QRect bounding_rect;
    TextAllign allign;
    QString font_tag;
    QColor color;
  };

  struct InfoTableColumnLayout {
    int left, width;
    TextAllign allign;
    QString font_tag;
    QColor color;
  };

  struct InfoTableLayout {
    QString name;
    int row_count, top, row_height;
    QVector<InfoTableColumnLayout> columns;

    inline void insertColumn(const InfoTableColumnLayout& layout) { columns.push_back(layout); }
  };

  struct PanelLayout {
    PanelId id;
    QString name;
    QRect geometry;
    int border_width;
    QColor back_color;
    QColor border_color;
    int text_count;

    QMap<QString, InfoTextLayout> texts;
    QMap<QString, InfoRectLayout> rects;
    QMap<QString, InfoTableLayout> tables;

    inline void insertRect(const InfoRectLayout& layout) { rects.insert(layout.name, layout); }
    inline void insertText(const InfoTextLayout& layout) { texts.insert(layout.name, layout); }
    inline void insertTable(const InfoTableLayout& layout) { tables.insert(layout.name, layout); }
  };


  struct CircleLayout {
    QPoint center;
    int radius;
    int mode_symb_shift;
    QString font;
    QRect box_rect;
  };

  struct MenuLayout {
    QRect geometry;
    QString font;
  };

  struct MagnifierLayout {
    QRect geometry;
  };

  struct Layout {
    QSize            size;
    CircleLayout     circle;
    MenuLayout       menu;
    MagnifierLayout  magnifier;

    QMap<PanelId, PanelLayout> panels;
    inline void insertPanel(const PanelLayout& layout) { panels.insert(layout.id, layout); }
  };

  class  LayoutManager {
  public:
     LayoutManager(const QString& filename);
    ~LayoutManager(void);

    inline const QSize& currentSize() const { return _currentSize; }
    bool resize(const QSize& size);

    inline const Layout& layout() { return _layouts[_currentSize]; }

  private:
    QHash<QSize, Layout> _layouts;
    QSize _currentSize;

    QMap<QString, QString> readXMLAttributes(QXmlStreamReader* xml);

    QColor              parseColor          (const QString& txt) const;
    QSize               parseSize           (const QString& txt) const;
    QPoint              parsePoint          (const QSize& scrn_sz, const QSize& sz, const QString& txt) const;
    QRect               parseRect           (const QString& txt) const;
    TextAllign          parseAllign         (const QString& txt) const;

    Layout              readLayout          (const QSize& scrn_sz, QXmlStreamReader* xml);

    CircleLayout        readCircleLayout    (const QSize& scrn_sz, QXmlStreamReader* xml);
    MenuLayout          readMenuLayout      (const QSize& scrn_sz, QXmlStreamReader* xml);
    MagnifierLayout     readMagnifierLayout (const QSize& scrn_sz, QXmlStreamReader* xml);

    PanelLayout     readPanelLayout (const QSize& scrn_sz, QXmlStreamReader* xml);

    InfoTextLayout         readInfoTextLayout        (PanelId panelId, QXmlStreamReader* xml);
    InfoRectLayout         readInfoRectLayout        (QXmlStreamReader* xml);
    InfoTableLayout        readInfoTableLayout       (QXmlStreamReader* xml);
    InfoTableColumnLayout  readInfoTableColumnLayout (QXmlStreamReader* xml);
  };
}

#endif // RLILAYOUT_H
