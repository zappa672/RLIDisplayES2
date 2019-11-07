#ifndef INFOBLOCK_H
#define INFOBLOCK_H

#include <QRect>
#include <QColor>

#include <map>
#include <vector>

#include "../texturelayerbase.h"
#include "../fonts.h"

#include "../../common/layout.h"
#include "../../common/strings.h"

namespace RLI {

  struct InfoRect {
    QColor color;
    QRect  geometry;
  };

  struct InfoText {
    StrId       str_id;
    QByteArray  value;

    QString font_tag;
    QColor  color;
    QRect   geometry;
    TextAllign allign;

    InfoText() {
      str_id = StrId::NONE;
      allign = TextAllign::CENTER;
    }
  };


  class InfoBlock : public TextureLayerBase {
    Q_OBJECT

  public:
    InfoBlock(PanelId panelId, const Layout& layout, QOpenGLContext* context, const Fonts* fonts, QObject* parent = nullptr);
    ~InfoBlock() override;

  public slots:
    void paint(const State& state, const Layout& layout) override;
    void resizeTexture(const Layout& layout) override;
    void clearTexture() override;

    inline const InfoRect& rect(const QString& name) { return _rects[name]; }
    inline const InfoText& text(size_t textId) { return _texts[textId]; }

    void setRect(const QString& rectId, const QRect& r);
    void setText(size_t textId, StrId str);
    void setText(size_t textId, const QByteArray& val);

    inline void forceUpdate() { _need_update = true; }

  private:
    void parseLayout(const PanelLayout& layout);

    void drawText(const State& state, const InfoText& text, const Fonts* fonts);
    void drawRect(const QRect& rect, const QColor& col);

    const Fonts* _fonts;

    PanelId _panel_id;

    std::map<QString, InfoRect> _rects;
    std::vector<InfoText> _texts;

    QColor  _back_color;
    QColor  _border_color;
    int     _border_width;

    bool    _need_update;


    enum SHADER_ATTRIBUTES {
      ATTR_POSITION = 0
    , ATTR_ORDER    = 1
    , ATTR_CHAR_VAL = 2
    , ATTR_COUNT    = 3 };
    enum SHADER_UNIFORMS
    { UNIF_MVP    = 0
    , UNIF_COLOR  = 1
    , UNIF_SIZE   = 2 };

    GLuint _vbo_ids[ATTR_COUNT];
  };

}

#endif // INFOBLOCK_H
