#-------------------------------------------------
#
# Project created by QtCreator 2019-09-18T23:21:38
#
#-------------------------------------------------

QT       += core gui widgets opengl concurrent

TARGET = RLIDisplayES2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
  src/common/radarscale.cpp \
  src/common/layout.cpp \
  src/common/math.cpp \
  src/common/state.cpp \
  src/common/strings.cpp \
  src/common/triangulate.cpp \
  src/layers/layerbase.cpp \
  src/layers/radar/radarengine.cpp \
  src/layers/radar/radarpalette.cpp \
  src/main.cpp \
  src/mainwidget.cpp \
  src/datasources/radardatasource.cpp \
  src/layers/fbolayerbase.cpp

HEADERS += \
  src/common/radarscale.h \
  src/common/layout.h \
  src/common/math.h \
  src/common/state.h \
  src/common/stringnames.h \
  src/common/strings.h \
  src/common/triangulate.h \
  src/layers/layerbase.h \
  src/layers/radar/radarengine.h \
  src/layers/radar/radarpalette.h \
  src/mainwidget.h \
  src/common/properties.h \
  src/datasources/radardatasource.h \  
  src/layers/fbolayerbase.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
  layouts.xml

RESOURCES += \
  res/shaders.qrc
