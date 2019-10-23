QT       += core
QT       -= gui
CONFIG += c++11
TARGET = ChooseBestModule
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/ChooseBestModule
MOC_DIR = ../../../output/ChooseBestModule

INCLUDEPATH += ../Common/src \
            ../Common/src/log \
            ../Common/src/info \
            ../Common/src/thread \
            ./src \
            ../Common/src/message \
            ../CoreModule/src \
            ../../../../thirdparty/include 
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core
}

HEADERS += \
    src/*.h

SOURCES += \
    src/*.cpp
