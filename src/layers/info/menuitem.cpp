#include "menuitem.h"

using namespace RLI;

MenuItem::MenuItem(StrId name, QObject* parent) : QObject(parent) {
  _enabled = true;
  _locked = false;
  _name = name;
}



MenuItemMenu::MenuItemMenu(StrId name, MenuItemMenu* parent) : MenuItem(name, parent) {
  _type = Type::MENU;
  _parent = parent;
}

MenuItemMenu::~MenuItemMenu() {
  qDeleteAll(_items);
}



MenuItemAction::MenuItemAction(StrId name, QObject* parent) : MenuItem(name, parent) {
  _type = Type::ACTION;
}

void MenuItemAction::action() {
  emit triggered();
}



MenuItemList::MenuItemList(StrId name, int def_ind, QObject* parent) : MenuItem(name, parent) {
  _type = Type::LIST;
  _index = def_ind;
}

void MenuItemList::up() {
  if (_index < _variants.size() - 1) {
    _index++;
    emit valueChanged(_variants[_index]);
  }
}

void MenuItemList::down() {
  if (_index > 0) {
    _index--;
    emit valueChanged(_variants[_index]);
  }
}



MenuItemInt::MenuItemInt(StrId name, int min, int max, int def, QObject* parent) : MenuItem(name, parent) {
  _type = Type::INT;

  _min = min;
  _max = max;
  _value = def;

  _change_start_time = QDateTime::currentDateTime().addSecs(-1);
  _last_change_time = QDateTime::currentDateTime().addSecs(-1);

  _delta=1;
}

void MenuItemInt::up() {
  if (_value < _max) {
    adjustDelta();

    _value += _delta;

    if (_value > _max)
      _value = _max;

    emit valueChanged(_value);
  }
}

void MenuItemInt::down() {
  if (_value > _min) {
    adjustDelta();

    _value -= _delta;

    if (_value < _min)
      _value = _min;

    emit valueChanged(_value);
  }
}

int MenuItemInt::setValue(int val) {
  if((val < _min) || (val > _max))
    return -2;

  _value = val;
  return 0;
}

int MenuItemInt::setValue(QByteArray val) {
  int  res;
  bool ok = false;

  res = val.toInt(&ok);
  if(!ok)
      return -1;
  if((res < _min) || (res > _max))
      return -2;
  _value = res;

  return 0;
}

void MenuItemInt::adjustDelta() {
  QDateTime currentTime = QDateTime::currentDateTime();

  if (currentTime > _last_change_time.addSecs(1)) {
    _last_change_time = currentTime;
    _change_start_time = currentTime;
    _delta = 1;
  } else {
    if (_change_start_time.msecsTo(currentTime) > 5000) {
      _delta = 100;
    } else if (_change_start_time.msecsTo(currentTime) > 3000) {
      _delta = 10;
    } else {
      _delta = 1;
    }

    _last_change_time = currentTime;
  }
}




MenuItemReal::MenuItemReal(StrId name, double min, double max, double def) : MenuItem(name) {
  _type = Type::REAL;
  _min = min;
  _max = max;
  _value = def;
  _step = 0.2f;
}

void MenuItemReal::up() {
  if (_value + _step < _max)
    _value += _step;
}

void MenuItemReal::down() {
  if (_value - _step > _min)
    _value -= _step;
}
