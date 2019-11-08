#include "infoblock.h"

#include <QDebug>
#include <QDateTime>

using namespace RLI;

InfoBlock::InfoBlock(PanelId panelId, const Layout& layout, QOpenGLContext* context, const Fonts* fonts, QObject* parent)
  : TextureLayerBase(layout.panels[panelId].geometry, context, parent), _fonts(fonts), _panel_id(panelId) {
  parseLayout(layout.panels[panelId]);

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

  _need_update = true;
}

InfoBlock::~InfoBlock() {
  glDeleteBuffers(ATTR_COUNT, _vbo_ids);
}

void InfoBlock::parseLayout(const PanelLayout& layout) {
  _back_color   = layout.back_color;
  _border_color = layout.border_color;
  _border_width = layout.border_width;

  for (const InfoRectLayout& rl : layout.rects) {
    InfoRect rect;
    rect.geometry = rl.rect;
    rect.color = rl.color;
    _rects[rl.name] = rect;
  }

  _texts = std::vector<InfoText>(static_cast<size_t>(layout.texts.size()));
  for (const InfoTextLayout& tl : layout.texts) {
    InfoText text;
    text.geometry = tl.bounding_rect;
    text.color = tl.color;
    text.allign = tl.allign;
    text.font_tag = tl.font_tag;
    text.str_id = _texts[tl.text_id].str_id;
    text.value = _texts[tl.text_id].value;

    _texts[tl.text_id] = text;
  }
}

void InfoBlock::setRect(const QString& rectId, const QRect& rect) {
  _rects[rectId].geometry = rect;
  _need_update = true;
}

void InfoBlock::setText(size_t textId, StrId str) {
  _texts[textId].str_id = str;
  _texts[textId].value.clear();
  _need_update = true;
}

void InfoBlock::setText(size_t textId, const QByteArray& val) {
  _texts[textId].str_id = StrId::NONE;
  _texts[textId].value = val;
  _need_update = true;
}



void InfoBlock::paint(const State& state, const Layout&) {
  if (!_need_update)
    return;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());
  glUseProgram(progId());

  glViewport(0.f, 0.f, width(), height());

  glUniformMatrix4fv(unifLoc(UNIF_MVP), 1, GL_FALSE, projection().data());

  glClearColor(_back_color.redF(), _back_color.greenF(), _back_color.blueF(), 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (_border_width >= 1) {
    drawRect(QRect(QPoint(0, 0), QSize(width(), _border_width)), _border_color);
    drawRect(QRect(QPoint(0, 0), QSize(_border_width, height())), _border_color);
    drawRect(QRect(QPoint(0, height() - _border_width), QSize(width(), _border_width)), _border_color);
    drawRect(QRect(QPoint(width() - _border_width, 0), QSize(_border_width, height())), _border_color);
  }

  for (auto rect : _rects)
    drawRect(rect.second.geometry, rect.second.color);

  for (auto text : _texts)
    drawText(state, text, _fonts);

  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _need_update = false;
}

void InfoBlock::resizeTexture(const Layout& layout) {
  TextureLayerBase::resize(layout.panels[_panel_id].geometry);
  parseLayout(layout.panels[_panel_id]);
  _need_update = true;
}

void InfoBlock::clearTexture() {
  TextureLayerBase::clear(1.f, 1.f, 1.f, 1.f, 0.f);
}


void InfoBlock::drawText(const State& state, const InfoText& text, const Fonts* fonts) {
  GLuint tex_id = fonts->texture(text.font_tag);
  QSize font_size = fonts->fontSize(text.font_tag);

  std::vector<GLfloat> pos;
  std::vector<GLfloat> ord, chars;

  QByteArray str;
  if (text.str_id == StrId::NONE)
    str = text.value;
  else
    str = Strings::instance().string(state.lang, text.str_id);

  QPoint anchor;
  switch (text.allign) {
    case TextAllign::LEFT:
      anchor = text.geometry.topLeft();
      break;
    case TextAllign::RIGHT:
      anchor = text.geometry.topRight() - QPoint(font_size.width()*str.size(), 0);
      break;
    case TextAllign::CENTER:
      anchor = text.geometry.center() - QPoint((font_size.width()*str.size()) / 2 - 1, font_size.height() / 2 - 1);
      break;
  }

  for (int i = 0; i < str.size(); i++) {
    QPoint lefttop = anchor + QPoint(i * font_size.width(), 0);
    for (int j = 0; j < 4; j++) {
      pos.push_back(lefttop.x());
      pos.push_back(lefttop.y());
      ord.push_back(j);
      chars.push_back(str[i]);
    }
  }


  glUniform2f(unifLoc(UNIF_SIZE), font_size.width(), font_size.height());
  glUniform4f(unifLoc(UNIF_COLOR), text.color.redF(), text.color.greenF(), text.color.blueF(), 1.f);


  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, static_cast<uint>(pos.size())*sizeof(GLfloat), pos.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_POSITION), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_POSITION));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, static_cast<uint>(ord.size())*sizeof(GLfloat), ord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_ORDER), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_ORDER));

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, static_cast<uint>(chars.size())*sizeof(GLfloat), chars.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_CHAR_VAL), 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_CHAR_VAL));


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, ord.size());
  glBindTexture(GL_TEXTURE_2D, 0);
}


void InfoBlock::drawRect(const QRect& rect, const QColor& col) {
  std::vector<GLfloat> pos;

  pos.push_back(rect.x());
  pos.push_back(rect.y());
  pos.push_back(rect.x());
  pos.push_back(rect.y() + rect.height());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y() + rect.height());

  glUniform2f(unifLoc(UNIF_SIZE), 0.f, 0.f);
  glUniform4f(unifLoc(UNIF_COLOR), col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, static_cast<uint>(pos.size())*sizeof(GLfloat), pos.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(attrLoc(ATTR_POSITION), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(attrLoc(ATTR_POSITION));

  glVertexAttrib1f(attrLoc(ATTR_ORDER), 0.f);
  glDisableVertexAttribArray(attrLoc(ATTR_ORDER));

  glVertexAttrib1f(attrLoc(ATTR_CHAR_VAL), 0.f);
  glDisableVertexAttribArray(attrLoc(ATTR_CHAR_VAL));

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
