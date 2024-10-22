QT       += core
QT       -= gui
CONFIG += c++11  plugin

TARGET = Common
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/Common
MOC_DIR = ../../../output/Common

INCLUDEPATH += ../../../../thirdparty/include \
            ../../../../thirdparty/include/Qt \
            ../../../../thirdparty/include/openCV \
            ../../protocol/log/src \
            src \
            src/common \
            src/info \
            src/log \
            src/perftool \
            src/thread \
            src/SystemInfo \
            src/config/StorageConfig \
            src/message

win32 {
LIBS += -L../../../../thirdparty/lib/windows64/curl -llibcurl \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/log4cplus -llog4cplusU \
        -L../../../../thirdparty/lib/windows64/openCV -lopencv_core400 -lopencv_highgui400 -lopencv_imgcodecs400 -lopencv_imgproc400 \
        -L../../../../thirdparty/lib/windows64/jsoncpp -ljsoncpp
        #-L../../../../thirdparty/lib/windows64/gperftools -laddressmap_unittest -llibtcmalloc_minimal -llow_level_alloc_unittest \
}

unix {
LIBS += -L../../../../thirdparty/lib/linux64/curl -lcurl \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/linux64/log4cplus -llog4cplus
        #-L../../../../thirdparty/lib/linux64/gperftools/static -lprofiler -ltcmalloc \
}

HEADERS += \
    src/*.h \
    src/common/*.h \
    src/message/*.h \
    src/info/*.h \
    src/log/*.h \
    src/thread/*.h \
    src/perftool/*.h \
    src/SystemInfo/*.h

SOURCES += \
    src/*.cpp \
    src/common/*.cpp \
    src/message/*.cpp \
    src/info/*.cpp \
    src/log/*.cpp \
    src/thread/*.cpp \
    src/perftool/*.cpp \
    src/SystemInfo/*.cpp
