QT       += core
QT       -= gui
CONFIG += c++11
TARGET = test
TEMPLATE = app
DESTDIR = ../../../bin/abstract
OBJECTS_DIR = ../../../output/TestModule
MOC_DIR = ../../../output/TestModule

INCLUDEPATH += ./src \
            ../TrackModule/src/OpencvExtra/face/samples \
            ../TrackModule/src/OpencvExtra/quality/samples \
            ../TrackModule/src/OpencvExtra/tracking/samples \
            ../TrackModule/src/OpencvExtra/face/include \
            ../TrackModule/src/OpencvExtra/quality/include \
            ../TrackModule/src/OpencvExtra/tracking/include \
            ../TrackModule/src/OpencvExtra/plot/include \
            ../Common/src \
            ../Common/src/log \
            ../Common/src/info \
            ../Common/src/thread \
            ../Common/src/message \
            ../CoreModule/src \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/openCV
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule -lTrackModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/openCV -lopencv_core400 -lopencv_video400 -lopencv_videoio400 -lopencv_highgui400 -lopencv_ml400 -lopencv_objdetect400 -lopencv_imgcodecs400 -lopencv_imgproc400 \
        -lopencv_flann400 -lopencv_gapi400 -lopencv_dnn400 -lopencv_photo400 -lopencv_calib3d400
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule -lTrackModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core
}

HEADERS += \
    src/*.h

SOURCES += \
    src/*.cpp
