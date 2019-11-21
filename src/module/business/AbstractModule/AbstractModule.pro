QT       += core
QT       -= gui
CONFIG += c++11  plugin
TARGET = AbstractModule
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/AbstractModel
MOC_DIR = ../../../output/AbstractModel

INCLUDEPATH += ./src \
            ./src/common \
            ../../base/Common/src \
            ../../base/Common/src/log \
            ../../base/Common/src/info \
            ../../base/Common/src/thread \
            ../../base/Common/src/message \
            ../../base/CoreModule/src \
            ../../../../thirdparty/include 
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../bin/abstract/lib/module -lQt5Core \
        -L../../../../thirdparty/lib/windows64/ffmpeg -lavutil \
        -L../../../../thirdparty/lib/windows64/jsoncpp -ljsoncpp
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/ffmpeg -lavutil
}

HEADERS += \
    src/*.h \
    src/common/*.h

SOURCES += \
    src/*.cpp \
    src/common/*.cpp
