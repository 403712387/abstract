QT       += core
QT       -= gui
CONFIG += c++11
TARGET = FFmpegIngestModule
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/FFmpegIngestModule
MOC_DIR = ../../../output/FFmpegIngestModule

INCLUDEPATH += ../Common/src \
            ../Common/src/info \
            ../Common/src/thread \
            ./src \
            ./src/processor \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/Qt/QtCore \
            ../../../../thirdparty/include/ffmpeg \
            ../Common/src/message \
            ../CoreModule/src \
            ../../../../thirdparty/include  \
            ../../../../thirdparty/include/openCV
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/ffmpeg -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale \
        -L../../../../thirdparty/lib/windows64/openCV -lopencv_core400 -lopencv_video400 -lopencv_videoio400 -lopencv_highgui400 -lopencv_ml400 -lopencv_objdetect400 -lopencv_imgcodecs400 -lopencv_imgproc400 \
        -lopencv_flann400 -lopencv_gapi400 -lopencv_dnn400 -lopencv_photo400 -lopencv_calib3d400
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/linux64/ffmpeg  -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale
}

HEADERS += \
    src/*.h \
    src/processor/*.h

SOURCES += \
    src/*.cpp \
    src/processor/*.cpp
