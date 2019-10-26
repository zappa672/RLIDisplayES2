#include "layout.h"

#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QRegExp>

using namespace RLI;

const QRegExp SIZE_RE = QRegExp("^\\d{3,4}x\\d{3,4}$");

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
        layout.insertPanel(readInfoPanelLayout(scrn_sz, xml));
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


InfoPanelLayout LayoutManager::readInfoPanelLayout(const QSize& scrn_sz, QXmlStreamReader* xml) {
  InfoPanelLayout layout;
  auto attrs = readXMLAttributes(xml);

  QSize size = parseSize(attrs["size"]);
  QPoint pos = parsePoint(scrn_sz, size, attrs["pos"]);

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
        layout.insertText(readInfoTextLayout(xml));

      if (xml->name() == "rect")
        layout.insertRect(readInfoRectLayout(xml));

      if (xml->name() == "table")
        layout.insertTable(readInfoTableLayout(xml));
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


InfoTextLayout LayoutManager::readInfoTextLayout(QXmlStreamReader* xml) {
  InfoTextLayout layout;
  auto attrs = readXMLAttributes(xml);

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
