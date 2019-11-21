QT       += core
QT       -= gui
CONFIG += c++11  plugin
TARGET = FacePositionModule
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/FacePositionModule
MOC_DIR = ../../../output/FacePositionModule

INCLUDEPATH += ./src \
            ./src/common \
            ./src/ObjectDetect \
            ./src/OpencvExtra/face/samples \
            ./src/OpencvExtra/quality/samples \
            ./src/OpencvExtra/tracking/samples \
            ./src/OpencvExtra/face/include \
            ./src/OpencvExtra/quality/include \
            ./src/OpencvExtra/tracking/include \
            ./src/OpencvExtra/plot/include \
            ./src/tracking \
            ../Common/src \
            ../Common/src/log \
            ../Common/src/info \
            ../Common/src/common \
            ../Common/src/thread \
            ../Common/src/message \
            ../CoreModule/src \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/openCV
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/facedetection -lfacedetection \
        -L../../../../thirdparty/lib/windows64/openCV -lopencv_core400 -lopencv_video400 -lopencv_videoio400 -lopencv_highgui400 -lopencv_ml400 -lopencv_objdetect400 -lopencv_imgcodecs400 -lopencv_imgproc400 \
        -lopencv_flann400 -lopencv_gapi400 -lopencv_dnn400 -lopencv_photo400 -lopencv_calib3d400
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core
}

HEADERS += \
    src/*.h

SOURCES += \
    src/*.cpp
