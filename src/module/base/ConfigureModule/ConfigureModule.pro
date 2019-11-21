QT       += core
QT       -= gui
CONFIG += c++11  plugin
TARGET = ConfigureModule
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/ConfigureModule
MOC_DIR = ../../../output/ConfigureModule
#QMAKE_LFLAGS += -fPIC

INCLUDEPATH += ../Common/src \
            ../Common/src/info \
            ./src/common \
            ../../protocol/log/src \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/Qt \
            ../Common/src/message \
            ../CoreModule/src
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/jsoncpp -ljsoncpp
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core \
        #-L../../../../thirdparty/lib/linux64/gperftools/static -lprofiler -ltcmalloc \
        -L../../../../thirdparty/lib/linux64/jsoncpp -ljsoncpp
}

HEADERS += \
    src/*.h \
    src/common/*.h

SOURCES += \
    src/*.cpp \
    src/common/*.cpp
