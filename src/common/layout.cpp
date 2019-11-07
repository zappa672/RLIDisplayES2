#include "layout.h"

#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QRegExp>

using namespace RLI;

const QRegExp SIZE_RE = QRegExp("^\\d{3,4}x\\d{3,4}$");


namespace RLI {
  static std::map<PanelId, std::map<QString, size_t>> PANEL_TEXTS_MAP = {
      {  PanelId::GAIN,     { { "text", PNL_GAIN_TEXT_ID      } }   }
    , {  PanelId::WATER,    { { "text", PNL_WATER_TEXT_ID     } }   }
    , {  PanelId::RAIN,     { { "text", PNL_RAIN_TEXT_ID      } }   }
    , {  PanelId::APCH,     { { "text", PNL_APCH_TEXT_ID      } }   }
    , {  PanelId::EMISSION, { { "text", PNL_EMISSION_TEXT_ID  } }   }

    , {  PanelId::LABEL5,       { { "text", PNL_LABEL5_TEXT_ID      } }   }
    , {  PanelId::BAND,         { { "text", PNL_BAND_TEXT_ID        } }   }
    , {  PanelId::ORIENTATION,  { { "text", PNL_ORIENTATION_TEXT_ID } }   }
    , {  PanelId::LABEL2,       { { "text", PNL_LABEL2_TEXT_ID      } }   }
    , {  PanelId::LABEL3,       { { "text", PNL_LABEL3_TEXT_ID      } }   }
    , {  PanelId::LABEL4,       { { "text", PNL_LABEL4_TEXT_ID      } }   }
    , {  PanelId::FPS,          { { "label", PNL_FPS_LABEL_TEXT_ID  }
                                , { "value", PNL_FPS_VALUE_TEXT_ID  } }   }

    , {  PanelId::SCALE, { { "scale1", PNL_SCALE_SCALE1_TEXT_ID }
                         , { "slash" , PNL_SCALE_SLASH_TEXT_ID  }
                         , { "scale2", PNL_SCALE_SCALE2_TEXT_ID }
                         , { "units" , PNL_SCALE_UNITS_TEXT_ID  } }   }
    , {  PanelId::VN, { { "header",    PNL_VN_HEADER_TEXT_ID }
                      , { "table_0_0", PNL_VN_TBL_0_0_TEXT_ID }
                      , { "table_0_1", PNL_VN_TBL_0_1_TEXT_ID }
                      , { "table_0_2", PNL_VN_TBL_0_2_TEXT_ID }
                      , { "table_1_0", PNL_VN_TBL_1_0_TEXT_ID }
                      , { "table_1_1", PNL_VN_TBL_1_1_TEXT_ID }
                      , { "table_1_2", PNL_VN_TBL_1_2_TEXT_ID } }   }
    , {  PanelId::VD, { { "header",    PNL_VD_HEADER_TEXT_ID }
                      , { "table_0_0", PNL_VD_TBL_0_0_TEXT_ID }
                      , { "table_0_1", PNL_VD_TBL_0_1_TEXT_ID } }   }
    , {  PanelId::COURSE, { { "table_0_0", PNL_COURSE_TBL_0_0_TEXT_ID }
                          , { "table_0_1", PNL_COURSE_TBL_0_1_TEXT_ID }
                          , { "table_0_2", PNL_COURSE_TBL_0_2_TEXT_ID }
                          , { "table_1_0", PNL_COURSE_TBL_1_0_TEXT_ID }
                          , { "table_1_1", PNL_COURSE_TBL_1_1_TEXT_ID }
                          , { "table_1_2", PNL_COURSE_TBL_1_2_TEXT_ID } }   }

    , {  PanelId::POSITION, { { "table_0_0", PNL_POSITION_TBL_0_0_TEXT_ID }
                            , { "table_0_1", PNL_POSITION_TBL_0_1_TEXT_ID }
                            , { "table_1_0", PNL_POSITION_TBL_1_0_TEXT_ID }
                            , { "table_1_1", PNL_POSITION_TBL_1_1_TEXT_ID } }  }
    , {  PanelId::BLANK, { }  }
    , {  PanelId::CLOCK, { { "label", PNL_CLOCK_LABEL_TEXT_ID }
                         , { "time" , PNL_CLOCK_TIME_TEXT_ID  } }   }

    , {  PanelId::DANGER, { { "label", PNL_DANGER_LABEL_TEXT_ID } }   }
    , {  PanelId::TAILS,  { { "table_0_0", PNL_TAILS_TBL_0_0_TEXT_ID }
                          , { "table_0_1", PNL_TAILS_TBL_0_1_TEXT_ID }
                          , { "table_0_2", PNL_TAILS_TBL_0_2_TEXT_ID } }  }
    , {  PanelId::DANGER_INFO,  { { "table_0_0", PNL_DANGER_INFO_TBL_0_0_TEXT_ID }
                                , { "table_0_1", PNL_DANGER_INFO_TBL_0_1_TEXT_ID }
                                , { "table_0_2", PNL_DANGER_INFO_TBL_0_2_TEXT_ID }
                                , { "table_1_0", PNL_DANGER_INFO_TBL_1_0_TEXT_ID }
                                , { "table_1_1", PNL_DANGER_INFO_TBL_1_1_TEXT_ID }
                                , { "table_1_2", PNL_DANGER_INFO_TBL_1_2_TEXT_ID } }  }
    , {  PanelId::VECTOR, { { "table_0_0", PNL_VECTOR_TBL_0_0_TEXT_ID }
                          , { "table_0_1", PNL_VECTOR_TBL_0_1_TEXT_ID }
                          , { "table_0_2", PNL_VECTOR_TBL_0_2_TEXT_ID } }   }
    , {  PanelId::TARGETS,  { { "header"   , PNL_TARGETS_HEADER_TEXT_ID  }
                            , { "current"  , PNL_TARGETS_CURRENT_TEXT_ID }
                            , { "count"    , PNL_TARGETS_COUNT_TEXT_ID   }
                            , { "table_0_0", PNL_TARGETS_TBL_0_0_TEXT_ID }
                            , { "table_0_1", PNL_TARGETS_TBL_0_1_TEXT_ID }
                            , { "table_0_2", PNL_TARGETS_TBL_0_2_TEXT_ID }
                            , { "table_1_0", PNL_TARGETS_TBL_1_0_TEXT_ID }
                            , { "table_1_1", PNL_TARGETS_TBL_1_1_TEXT_ID }
                            , { "table_1_2", PNL_TARGETS_TBL_1_2_TEXT_ID }
                            , { "table_2_0", PNL_TARGETS_TBL_2_0_TEXT_ID }
                            , { "table_2_1", PNL_TARGETS_TBL_2_1_TEXT_ID }
                            , { "table_2_2", PNL_TARGETS_TBL_2_2_TEXT_ID }
                            , { "table_3_0", PNL_TARGETS_TBL_3_0_TEXT_ID }
                            , { "table_3_1", PNL_TARGETS_TBL_3_1_TEXT_ID }
                            , { "table_3_2", PNL_TARGETS_TBL_3_2_TEXT_ID }
                            , { "table_4_0", PNL_TARGETS_TBL_4_0_TEXT_ID }
                            , { "table_4_1", PNL_TARGETS_TBL_4_1_TEXT_ID }
                            , { "table_4_2", PNL_TARGETS_TBL_4_2_TEXT_ID }
                            , { "table_5_0", PNL_TARGETS_TBL_5_0_TEXT_ID }
                            , { "table_5_1", PNL_TARGETS_TBL_5_1_TEXT_ID }
                            , { "table_5_2", PNL_TARGETS_TBL_5_2_TEXT_ID }
                            , { "table_6_0", PNL_TARGETS_TBL_6_0_TEXT_ID }
                            , { "table_6_1", PNL_TARGETS_TBL_6_1_TEXT_ID }
                            , { "table_6_2", PNL_TARGETS_TBL_6_2_TEXT_ID }
                            , { "table_7_0", PNL_TARGETS_TBL_7_0_TEXT_ID }
                            , { "table_7_1", PNL_TARGETS_TBL_7_1_TEXT_ID }
                            , { "table_7_2", PNL_TARGETS_TBL_7_2_TEXT_ID } }  }
    , {  PanelId::CURSOR, { { "header"   , PNL_CURSOR_HEADER_TEXT_ID }
                          , { "table_0_0", PNL_CURSOR_TBL_0_0_TEXT_ID }
                          , { "table_0_1", PNL_CURSOR_TBL_0_1_TEXT_ID }
                          , { "table_0_2", PNL_CURSOR_TBL_0_2_TEXT_ID }
                          , { "table_1_0", PNL_CURSOR_TBL_1_0_TEXT_ID }
                          , { "table_1_1", PNL_CURSOR_TBL_1_1_TEXT_ID }
                          , { "table_1_2", PNL_CURSOR_TBL_1_2_TEXT_ID } } }
  };
};

LayoutManager:: LayoutManager(const QString& filename) {
  QFile file(filename);
  file.open(QFile::ReadOnly);

  QXmlStreamReader* xml = new QXmlStreamReader(&file);

  while (!xml->atEnd()) {
    if (xml->readNext() == QXmlStreamReader::StartElement) {
      QMap<QString, QString> attrs = readXMLAttributes(xml);

      if (xml->name() == "layouts")
        _currentSize = parseSize(attrs.value("default", "0x0"));

      if (xml->name() == "layout")
        _layouts.insert(parseSize(attrs["size"]), readLayout(parseSize(attrs["size"]), xml));
    }
  }

  file.close();

  if (_currentSize.isEmpty())
    _currentSize = _layouts.keys().first();
}

LayoutManager::~LayoutManager() {
}

bool LayoutManager::resize(const QSize& size) {
  int maxArea = 0;
  QSize best(0, 0);

  for (auto sz : _layouts.keys()) {
    if (sz.width() <= size.width() && sz.height() <= size.height()) {
      int area(sz.width() * sz.height());

      if (area > maxArea) {
        best = sz;
        maxArea = area;
      }
    }
  }

  if (!best.isEmpty())
    _currentSize = best;
  return !best.isEmpty();
}

QColor LayoutManager::parseColor(const QString& txt) const {
  QStringList slsize = txt.split(",");
  return QColor(slsize[0].toInt(), slsize[1].toInt(), slsize[2].toInt(), slsize[3].toInt());
}

QSize LayoutManager::parseSize(const QString& text) const {
  QStringList slsize = text.split("x");
  return QSize(slsize[0].toInt(), slsize[1].toInt());
}

QPoint LayoutManager::parsePoint(const QSize& scrn_sz, const QSize& sz, const QString& text) const {
  QStringList slpoint = text.split(",");
  int x = slpoint[0].replace("[^0-9]", "").toInt();
  int y = slpoint[1].replace("[^0-9]", "").toInt();

  if (x < 0) x += scrn_sz.width()  - sz.width();
  if (y < 0) y += scrn_sz.height() - sz.height();

  return QPoint(x, y);
}

QRect LayoutManager::parseRect(const QString& text) const {
  QStringList slsize = text.split("x");
  return QRect( QPoint(slsize[0].toInt(), slsize[1].toInt())
              , QSize(slsize[2].toInt(), slsize[3].toInt())  );
}

TextAllign LayoutManager::parseAllign(const QString& txt) const {
  if (txt == "left")
    return TextAllign::LEFT;

  if (txt == "right")
    return TextAllign::RIGHT;

  return TextAllign::CENTER;
}


QMap<QString, QString> LayoutManager::readXMLAttributes(QXmlStreamReader* xml) {
  QMap<QString, QString> attributes;

  for (QXmlStreamAttribute attr : xml->attributes())
    attributes.insert(attr.name().toString(), attr.value().toString());

  return attributes;
}


Layout LayoutManager::readLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  Layout layout;
  layout.size = scrn_sz;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {

    case QXmlStreamReader::StartElement:
      if (xml->name() == "circle")
        layout.circle = readCircleLayout(scrn_sz, xml);

      if (xml->name() == "menu")
        layout.menu = readMenuLayout(scrn_sz, xml);

      if (xml->name() == "magnifier")
        layout.magnifier = readMagnifierLayout(scrn_sz, xml);

      if (xml->name() == "panel")
        layout.insertPanel(readPanelLayout(scrn_sz, xml));
      break;

    case QXmlStreamReader::EndElement:
      if (xml->name() == "layout")
        return layout;
      break;

    default:
      break;
    }
  }

  return layout;
}


CircleLayout LayoutManager::readCircleLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  CircleLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.center = parsePoint(scrn_sz, scrn_sz, attrs["center"]);
  layout.radius = attrs["radius"].toInt();
  layout.font = attrs["font"];
  layout.mode_symb_shift = attrs["mode_symb_shift"].toInt();

  QPoint topLeft = layout.center - QPoint(layout.radius, layout.radius);
  QSize rectSize(2*layout.radius-1, 2*layout.radius-1);

  layout.box_rect = QRect(topLeft, rectSize);

  return layout;
}

MenuLayout LayoutManager::readMenuLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  MenuLayout layout;
  auto attrs = readXMLAttributes(xml);

  QSize size = parseSize(attrs["size"]);
  QPoint pos = parsePoint(scrn_sz, size, attrs["pos"]);

  layout.geometry = QRect(pos, size);
  layout.font = attrs["font"];

  return layout;
}

MagnifierLayout LayoutManager::readMagnifierLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  MagnifierLayout layout;
  auto attrs = readXMLAttributes(xml);

  QSize size = parseSize(attrs["size"]);
  QPoint pos = parsePoint(scrn_sz, size, attrs["pos"]);

  layout.geometry = QRect(pos, size);

  return layout;
}


PanelLayout LayoutManager::readPanelLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  PanelLayout layout;
  auto attrs = readXMLAttributes(xml);

  QSize size = parseSize(attrs["size"]);
  QPoint pos = parsePoint(scrn_sz, size, attrs["pos"]);

  static std::map<QString, PanelId> panels_map
  { { "gain",         PanelId::GAIN }
  , { "water",        PanelId::WATER }
  , { "rain",         PanelId::RAIN }
  , { "apch",         PanelId::APCH }
  , { "emission",     PanelId::EMISSION }

  , { "label5",       PanelId::LABEL5 }
  , { "band",         PanelId::BAND }
  , { "orientation",  PanelId::ORIENTATION }
  , { "label2",       PanelId::LABEL2 }
  , { "label3",       PanelId::LABEL3 }
  , { "label4",       PanelId::LABEL4 }
  , { "fps",          PanelId::FPS }

  , { "scale",        PanelId::SCALE }
  , { "vn",           PanelId::VN }
  , { "vd",           PanelId::VD }
  , { "course",       PanelId::COURSE }

  , { "position",     PanelId::POSITION }
  , { "blank",        PanelId::BLANK }
  , { "clock",        PanelId::CLOCK }

  , { "danger",       PanelId::DANGER }
  , { "tails",        PanelId::TAILS }
  , { "danger-info",  PanelId::DANGER_INFO }
  , { "vector",       PanelId::VECTOR }
  , { "targets",      PanelId::TARGETS }
  , { "cursor",       PanelId::CURSOR } };

  layout.id = panels_map[attrs["name"]];
  layout.name = attrs["name"];
  layout.geometry = QRect(pos, size);
  layout.border_width = attrs["border_width"].toInt();
  layout.border_color = parseColor(attrs["border_color"]);
  layout.back_color = parseColor(attrs["back_color"]);
  layout.text_count = attrs["text_count"].toInt();

  while (!xml->atEnd()) {
    switch (xml->readNext()) {

    case QXmlStreamReader::StartElement:
      if (xml->name() == "text")
        layout.insertText(readInfoTextLayout(layout.id, xml));

      if (xml->name() == "rect")
        layout.insertRect(readInfoRectLayout(xml));

      if (xml->name() == "table") {
          InfoTableLayout tl = readInfoTableLayout(xml);

          for (int i = 0; i < tl.row_count; i++) {
            for (int j = 0; j < tl.columns.size(); j++) {
              QString text_name = tl.name + "_" + QString::number(i) + "_" + QString::number(j);
              size_t text_id = PANEL_TEXTS_MAP[layout.id][text_name];

              QPoint topLeft(tl.columns[j].left, tl.top + i * tl.row_height);
              QSize size(tl.columns[j].width, tl.row_height);

              InfoTextLayout tLayout;
              tLayout.text_id = text_id;
              tLayout.name = text_name;
              tLayout.bounding_rect = QRect(topLeft, size);
              tLayout.allign = tl.columns[j].allign;;
              tLayout.font_tag = tl.columns[j].font_tag;
              tLayout.color = tl.columns[j].color;
              layout.insertText(tLayout);
            }
          }
      }
      break;

    case QXmlStreamReader::EndElement:
      if (xml->name() == "panel")
        return layout;
      break;

    default:
      break;
    }
  }

  return layout;
}


InfoTextLayout LayoutManager::readInfoTextLayout(PanelId panelId, QXmlStreamReader* xml) {
  InfoTextLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.text_id = PANEL_TEXTS_MAP[panelId].at(attrs["name"]);
  layout.name = attrs["name"];
  layout.bounding_rect = parseRect(attrs["rect"]);
  layout.allign = parseAllign(attrs["allign"]);
  layout.font_tag = attrs["font"];
  layout.color = parseColor(attrs["color"]);

  return layout;
}

InfoRectLayout LayoutManager::readInfoRectLayout(QXmlStreamReader* xml) {
  InfoRectLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.name = attrs["name"];
  layout.rect = parseRect(attrs["rect"]);
  layout.color = parseColor(attrs["color"]);

  return layout;
}

InfoTableLayout LayoutManager::readInfoTableLayout(QXmlStreamReader* xml) {
  InfoTableLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.name = attrs["name"];
  layout.top = attrs["top"].toInt();
  layout.row_height = attrs["row_height"].toInt();
  layout.row_count = attrs["row_count"].toInt();

  while (!xml->atEnd()) {
    switch (xml->readNext()) {

    case QXmlStreamReader::StartElement:
      if (xml->name() == "column")
        layout.insertColumn(readInfoTableColumnLayout(xml));
      break;

    case QXmlStreamReader::EndElement:
      if (xml->name() == "table")
        return layout;
      break;

    default:
      break;
    }
  }

  return layout;
}

InfoTableColumnLayout LayoutManager::readInfoTableColumnLayout(QXmlStreamReader* xml) {
  InfoTableColumnLayout layout;
  auto attrs = readXMLAttributes(xml);

  layout.left = attrs["left"].toInt();
  layout.width = attrs["width"].toInt();
  layout.allign = parseAllign(attrs["allign"]);
  layout.font_tag = attrs["font"];
  layout.color = parseColor(attrs["color"]);

  return layout;
}
