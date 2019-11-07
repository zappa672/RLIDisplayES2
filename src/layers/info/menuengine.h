#ifndef MENUENGINE_H
#define MENUENGINE_H

#include <QDebug>
#include <QDateTime>
#include <QByteArray>
#include <QKeyEvent>

#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>

#include "../fonts.h"
#include "menuitem.h"

#include "../../common/strings.h"
#include "../../common/state.h"
#include "../../common/layout.h"

#include "../texturelayerbase.h"

namespace RLI {

  class MenuEngine : public TextureLayerBase {
    Q_OBJECT
    Q_ENUM(StrId)
  public:
    MenuEngine(const Layout& layout, const Fonts* fonts, QOpenGLContext* context, QObject* parent = nullptr);
    virtual ~MenuEngine() override;

  signals:
    void languageChanged(StrId lang_str);
    void radarBrightnessChanged(int br);
    void simulationChanged(StrId lang_str);

    void startRouteEdit();
    void finishRouteEdit();

    void saveRoute(int index);
    void loadRoute(int index);

    void analogZeroChanged(int val);
    void tailsModeChanged(StrId lang_str);
    void bandModeChanged(StrId lang_str);

  public slots:
    void onStateChanged(WidgetState state);
    void onKeyPressed(QKeyEvent* event);

    void paint(const State& state, const Layout& layout) override;
    void resizeTexture(const Layout& layout) override;
    void clearTexture() override;

    void onUp();
    void onDown();
    void onEnter();
    void onBack();

    void onAnalogZeroChanged(int val);

  private:
    void initMainMenuTree();
    void initCnfgMenuTree();

    enum TextAllignement { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };

    void drawBar();
    void drawSelection();

    void drawRect(const QRect& rect, const QColor& col);
    void drawText(const QByteArray& text, int line, TextAllignement align, const QColor& col);

    bool _need_update;

    QString _font_tag;
    QDateTime _last_action_time;

    MenuItemMenu* _menu;

    MenuItemMenu* _main_menu;
    MenuItemMenu* _cnfg_menu;

    MenuItemAction* routeEditItem;
    MenuItemInt* routeLoaderItem;
    MenuItemInt* routeSaverItem;

    MenuItemInt* analogZeroItem;

    int _selected_line;
    bool _selection_active;

    const Fonts* _fonts;

    // -----------------------------------------------
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

};

#endif // MENUENGINE_H
