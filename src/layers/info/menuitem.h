#ifndef MENUITEM_H
#define MENUITEM_H

#include <QDebug>
#include <QDateTime>
#include <QByteArray>

#include "../../common/strings.h"

namespace RLI {

  class MenuItem : public QObject {
    Q_OBJECT
  public:
    MenuItem(StrId name, QObject* parent = nullptr);
    virtual ~MenuItem() { }

    enum class Type { MENU, LIST, INT, REAL, ACTION };
    virtual Type type() { return _type; }

    QByteArray name(Lang lang) const { return Strings::instance().string(lang, _name); }
    virtual QByteArray value(Lang) const { return QByteArray(); }

    inline bool enabled() const { return _enabled; }
    inline void setEnabled(bool val) { _enabled = val; }

    inline bool locked() const { return _locked; }
    inline void setLocked(bool val) { _locked = val; }

    virtual void up() { }
    virtual void down() { }
    virtual void action() { }

  protected:
    Type _type;

    bool _enabled;
    bool _locked;

  private:
    StrId _name;
  };


  class MenuItemAction : public MenuItem {
    Q_OBJECT
  public:
    MenuItemAction(StrId name, QObject* parent = nullptr);
    ~MenuItemAction() override {}

    void action() override;

  signals:
    void triggered();

  private:
  };


  class MenuItemMenu : public MenuItem {
  public:
    MenuItemMenu(StrId name, MenuItemMenu* parent);
    ~MenuItemMenu() override;

    inline MenuItemMenu* parent() const { return _parent; }
    inline MenuItem* item(int i) const { return _items[i]; }
    inline int item_count() const { return _items.size(); }

    inline void add_item(MenuItem* i) { _items.push_back(i); }

  private:
    MenuItemMenu* _parent;
    QVector<MenuItem*> _items;
  };


  class MenuItemList : public MenuItem {
    Q_OBJECT
  public:
    MenuItemList(StrId name, int def_ind, QObject* parent = nullptr);
    ~MenuItemList() override {}

    inline QByteArray value(Lang lang) const override { return Strings::instance().string(lang, _variants[_index]); }
    inline void addVariant(StrId value) { _variants.push_back(value); }

    void up() override;
    void down() override;

  signals:
    void valueChanged(StrId);

  private:
    int _index;
    QVector<StrId> _variants;
  };


  class MenuItemInt : public MenuItem {
      Q_OBJECT
  public:
    MenuItemInt(StrId name, int min, int max, int def, QObject* parent = nullptr);
    ~MenuItemInt() override {}

    inline QByteArray value(Lang) const override { return QString::number(_value).toLocal8Bit(); }
    inline const int& intValue() const { return _value; }
    inline const int& minValue() const { return _min; }
    inline const int& maxValue() const { return _max; }

    void up() override;
    void down() override;

  public slots:
    int setValue(int val);
    int setValue(QByteArray val);

  signals:
    void valueChanged(int);

  private:
    void adjustDelta();

    QDateTime _change_start_time;
    QDateTime _last_change_time;

    int _delta;

    int _value;
    int _min, _max;
  };


  class MenuItemReal : public MenuItem {
  public:
    MenuItemReal(StrId name, double min, double max, double def);
    ~MenuItemReal() override {}

    inline QByteArray value(Lang) const override { return QString::number(_value).left(5).toLocal8Bit(); }
    inline const double& fltValue() const { return _value; }
    inline const double& minValue() const { return _min; }
    inline const double& maxValue() const { return _max; }


    void up() override;
    void down() override;

  private:
    double _value;
    double _step;
    double _min, _max;
  };

};

#endif // MENUITEM_H
