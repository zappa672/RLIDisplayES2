#ifndef RLISTRINGS_H
#define RLISTRINGS_H

#include <QMap>
#include <QVector>
#include <QByteArray>
#include <QTextEncoder>

#include "stringnames.h"

namespace RLI {

  enum class RLILang : int {
    ENGLISH  = 0
  , RUSSIAN  = 1
  , COUNT    = 2 };

  class RLIStrings {
  public:
    static RLIStrings& instance() {
      static RLIStrings instance;
      return instance;
    }

    const QByteArray string(RLILang lang, RLIString name) const { return _strings[name][static_cast<int>(lang)]; }
    QTextEncoder* encoder() const { return _encoder; }

  private:
    RLIStrings(void);
    ~RLIStrings(void);

    // Singleton
    RLIStrings(RLIStrings const&) = delete;
    RLIStrings& operator= (RLIStrings const&) = delete;

    QTextEncoder* _encoder;
    void addString(RLIString name, QVector<QString> vals);

    QMap<RLIString, QVector<QByteArray>> _strings;
  };

}

#endif // RLISTRINGS_H
