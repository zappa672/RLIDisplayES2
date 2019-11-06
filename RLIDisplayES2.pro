#-------------------------------------------------
#
# Project created by QtCreator 2019-09-18T23:21:38
#
#-------------------------------------------------

QT       += core gui widgets opengl concurrent

TARGET = RLIDisplayES2
TEMPLATE = app


unix:QMAKE_CXXFLAGS += -Wno-write-strings
unix:QMAKE_CXXFLAGS += -Wno-unused-variable
unix:QMAKE_CXXFLAGS += -std=gnu++11


# include gdal
# win32:QMAKE_LIBDIR += C:/GDAL/lib
# win32:INCLUDEPATH += C:/GDAL/include
# win32:LIBS += -lgdal_i -lgeos_i
# unix:LIBS += -lgdal  -lrt


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
  src/datasources/datasourcebase.cpp \
  src/layers/info/infoblock.cpp \
  src/layers/info/infoengine.cpp \
  src/layers/info/menuengine.cpp \
  src/layers/info/menuitem.cpp \
  src/layers/texturelayerbase.cpp \
  src/main.cpp \
  src/mainwidget.cpp \
  src/common/layout.cpp \
  src/common/math.cpp \
  src/common/radarscale.cpp \
  src/common/state.cpp \
  src/common/strings.cpp \
  src/common/triangulate.cpp \
  src/datasources/radardatasource.cpp \
  src/datasources/shipdatasource.cpp \
  src/datasources/targetdatasource.cpp \
  src/layers/fonts.cpp \
  src/layers/layerbase.cpp \
  src/layers/magnifierengine.cpp \
  src/layers/maskengine.cpp \
  src/layers/radar/radarengine.cpp \
  src/layers/radar/radarpalette.cpp

HEADERS += \
  src/common/structures.h \
  src/datasources/datasourcebase.h \
  src/layers/info/infoblock.h \
  src/layers/info/infoengine.h \
  src/layers/info/menuengine.h \
  src/layers/info/menuitem.h \
  src/layers/texturelayerbase.h \
  src/mainwidget.h \
  src/common/layout.h \
  src/common/math.h \
  src/common/properties.h \
  src/common/radarscale.h \
  src/common/state.h \
  src/common/strings.h \
  src/common/triangulate.h \
  src/datasources/radardatasource.h \
  src/datasources/shipdatasource.h \
  src/datasources/targetdatasource.h \
  src/layers/fonts.h \
  src/layers/layerbase.h \
  src/layers/magnifierengine.h \
  src/layers/maskengine.h \
  src/layers/radar/radarengine.h \
  src/layers/radar/radarpalette.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
  assets/pelengs/r1nm3h0_4096 \
  assets/pelengs/r1nm6h0_4096 \
  assets/textures/fonts/F12X14B.png \
  assets/textures/fonts/F14X14B.png \
  assets/textures/fonts/F16X14B.png \
  assets/textures/fonts/F16X28B.png \
  assets/textures/fonts/F16X32B.png \
  assets/textures/fonts/F16X40B.png \
  assets/textures/fonts/F8X14B.png \
  assets/textures/fonts/F8X8B.png \
  layouts.xml

RESOURCES += \
  res/shaders.qrc
