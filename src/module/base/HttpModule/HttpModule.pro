QT       += core
QT       -= gui
CONFIG += c++11 plugin
TARGET = HttpModule
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/HttpModule
MOC_DIR = ../../../output/HttpModule
#QMAKE_LFLAGS += -fPIC

INCLUDEPATH += ../Common/src \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/Qt \
            ../Common/src/message \
            ../Common/src/info \
            ../Common/src/common \
            ../Common/src/SystemInfo \
            ./src \
            ./src/helper \
            ../CoreModule/src

win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/libmicrohttpd -llibmicrohttpd-dll \
        -L../../../../thirdparty/lib/windows64/jsoncpp -ljsoncpp
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core \
        #-L../../../../thirdparty/lib/linux64/gperftools/static -lprofiler -ltcmalloc \
        -L../../../../thirdparty/lib/linux64/libmicrohttpd  -lmicrohttpd \
        -L../../../../thirdparty/lib/linux64/jsoncpp  -ljsoncpp
}

HEADERS += \
    src/*.h \
    src/helper/*.h

SOURCES += \
    src/*.cpp \
    src/helper/*.cpp
