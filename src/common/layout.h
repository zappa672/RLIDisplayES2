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

  enum class TextAllign { LEFT, RIGHT, CENTER };

  struct InfoRectLayout {
    QString name;
    QRect   rect;
    QColor  color;
  };

  struct InfoTextLayout {
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

  struct InfoPanelLayout {
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

    QMap<QString, InfoPanelLayout> panels;
    inline void insertPanel(const InfoPanelLayout& layout) { panels.insert(layout.name, layout); }
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

    InfoPanelLayout     readInfoPanelLayout (const QSize& scrn_sz, QXmlStreamReader* xml);

    InfoTextLayout         readInfoTextLayout        (QXmlStreamReader* xml);
    InfoRectLayout         readInfoRectLayout        (QXmlStreamReader* xml);
    InfoTableLayout        readInfoTableLayout       (QXmlStreamReader* xml);
    InfoTableColumnLayout  readInfoTableColumnLayout (QXmlStreamReader* xml);
  };
}

#endif // RLILAYOUT_H
