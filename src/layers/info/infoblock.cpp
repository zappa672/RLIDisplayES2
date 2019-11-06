#include "infoblock.h"

#include <QDebug>
#include <QDateTime>

using namespace RLI;

InfoBlock::InfoBlock(PanelId panelId, const Layout& layout, QOpenGLContext* context, const Fonts* fonts, QObject* parent)
  : TextureLayerBase(layout.panels[panelId].geometry, context, parent), _fonts(fonts), _panel_id(panelId) {

  parseLayout(layout.panels[panelId]);

  _projection.setToIdentity();
  _projection.ortho(0.f, width(), 0.f, height(), -1.f, 1.f);

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

}

void InfoBlock::setRect(const QString& rectId, const QRect& rect) {
  //_rects[rectId].geometry = rect;
  //_need_update = true;
}

void InfoBlock::setText(size_t textId, StrId str) {
  //_texts[textId].str_id = str;
  //_texts[textId].value.clear();
  //_need_update = true;
}

void InfoBlock::setText(size_t textId, const QByteArray& val) {
  //_texts[textId].str_id = StrId::NONE;
  //_texts[textId].value = val;
  //_need_update = true;
}



void InfoBlock::paint(const State&, const Layout&) {
  if (!_need_update)
    return;

  glBindFramebuffer(GL_FRAMEBUFFER, fboId());

  glDisable(GL_DEPTH_TEST);

  glViewport(0.f, 0.f, width(), height());

  glUseProgram(progId());

  glClearColor(0.12f, 0.13f, 0.10f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  //_prog->setUniformValue(_uniform_locs[INFO_UNIF_MVP], _projection);
  //
  //updateBlock(block, fonts);

  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  _need_update = false;
}

void InfoBlock::resizeTexture(const Layout& layout) {
  TextureLayerBase::resize(QRect(QPoint(0,0), layout.size));
  parseLayout(layout.panels[_panel_id]);
  _projection.setToIdentity();
  _projection.ortho(0.f, width(), 0.f, height(), -1.f, 1.f);
  _need_update = true;
}

void InfoBlock::clearTexture() {
  TextureLayerBase::clear(1.f, 1.f, 1.f, 0.f, 0.f);
}



/*
void InfoEngine::updateBlock(InfoBlock* b, InfoFonts* fonts) {
  QColor bckCol = b->backColor();
  QColor brdCol = b->borderColor();
  int    brdWid = b->borderWidth();
  QRect  geom   = QRect(QPoint(0, 0), b->geometry().size());

  glClearColor(bckCol.redF(), bckCol.greenF(), bckCol.blueF(), bckCol.alphaF());
  glClear(GL_COLOR_BUFFER_BIT);

  if (brdWid >= 1) {
    drawRect(QRect(QPoint(0, 0), QSize(geom.width(), brdWid)), brdCol);
    drawRect(QRect(QPoint(0, 0), QSize(brdWid, geom.height())), brdCol);
    drawRect(QRect(QPoint(0, geom.height() - brdWid), QSize(geom.width(), brdWid)), brdCol);
    drawRect(QRect(QPoint(geom.width() - brdWid, 0), QSize(brdWid, geom.height())), brdCol);
  }

  for (const InfoRect& rect : b->rectangles())
    drawRect(rect.geometry, rect.color);

  for (const InfoText& text : b->texts())
    drawText(text, fonts);

  b->clearUpdate();
}

void InfoEngine::drawText(const State& state, const InfoText& text, Fonts* fonts) {
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


  glUniform2f(_uniform_locs[INFO_UNIF_SIZE], font_size.width(), font_size.height());
  glUniform4f(_uniform_locs[INFO_UNIF_COLOR], text.color.redF(), text.color.greenF(), text.color.blueF(), 1.f);


  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_ORDER]);
  glBufferData(GL_ARRAY_BUFFER, ord.size()*sizeof(GLfloat), ord.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_ORDER], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_CHAR_VAL]);
  glBufferData(GL_ARRAY_BUFFER, chars.size()*sizeof(GLfloat), chars.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_CHAR_VAL], 1, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_CHAR_VAL]);


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, ord.size());
  glBindTexture(GL_TEXTURE_2D, 0);
}


void InfoEngine::drawRect(const QRect& rect, const QColor& col) {
  std::vector<GLfloat> pos;

  pos.push_back(rect.x());
  pos.push_back(rect.y());
  pos.push_back(rect.x());
  pos.push_back(rect.y() + rect.height());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y());
  pos.push_back(rect.x() + rect.width());
  pos.push_back(rect.y() + rect.height());

  glUniform2f(_uniform_locs[INFO_UNIF_SIZE], 0.f, 0.f);
  glUniform4f(_uniform_locs[INFO_UNIF_COLOR], col.redF(), col.greenF(), col.blueF(), col.alphaF());

  glBindBuffer(GL_ARRAY_BUFFER, _vbo_ids[INFO_ATTR_POSITION]);
  glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(GLfloat), pos.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(_attr_locs[INFO_ATTR_POSITION], 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid*>(0));
  glEnableVertexAttribArray(_attr_locs[INFO_ATTR_POSITION]);

  glVertexAttrib1f(_attr_locs[INFO_ATTR_ORDER], 0.f);
  glDisableVertexAttribArray(_attr_locs[INFO_ATTR_ORDER]);

  glVertexAttrib1f(_attr_locs[INFO_ATTR_CHAR_VAL], 0.f);
  glDisableVertexAttribArray(_attr_locs[INFO_ATTR_CHAR_VAL]);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
*/
