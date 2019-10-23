QT       += core
QT       -= gui
CONFIG += c++11
TARGET = TrackModule
TEMPLATE = lib
DESTDIR = ../../../bin/abstract/lib/module
OBJECTS_DIR = ../../../output/TrackModule
MOC_DIR = ../../../output/TrackModule

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
            ../Common/src/thread \
            ../Common/src/message \
            ../CoreModule/src \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/openCV
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/openCV -lopencv_core400 -lopencv_video400 -lopencv_videoio400 -lopencv_highgui400 -lopencv_ml400 -lopencv_objdetect400 -lopencv_imgcodecs400 -lopencv_imgproc400 \
        -lopencv_flann400 -lopencv_gapi400 -lopencv_dnn400 -lopencv_photo400 -lopencv_calib3d400
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core
}

HEADERS += \
    src/*.h \
    ./src/common/*.h \
    ./src/ObjectDetect/*.h \
    ./src/tracking/*.h \
    ./src/OpencvExtra/face/src/*.hpp \
    ./src/OpencvExtra/quality/src/*.hpp \
    ./src/OpencvExtra/tracking/src/*.hpp \
    ./src/OpencvExtra/plot/src/*.hpp \
    ./src/OpencvExtra/plot/include/opencv2/*.hpp \
    ./src/OpencvExtra/face/include/opencv2/*.hpp \
    ./src/OpencvExtra/face/samples/*.h \
    ./src/OpencvExtra/quality/samples/*.h \
    ./src/OpencvExtra/tracking/samples/*.h \
    ./src/OpencvExtra/face/include/opencv2/face/*.hpp \
    ./src/OpencvExtra/tracking/include/opencv2/*.hpp \
    ./src/OpencvExtra/tracking/include/opencv2/tracking/*.hpp \
    ./src/OpencvExtra/quality/include/opencv2/*.hpp \
    ./src/OpencvExtra/quality/include/opencv2/quality/*.hpp

SOURCES += \
    src/*.cpp \
    ./src/common/*.cpp \
    ./src/ObjectDetect/*.cpp \
    ./src/tracking/*.cpp \
    ./src/OpencvExtra/face/src/*.cpp \
    ./src/OpencvExtra/face/samples/*.cpp \
    ./src/OpencvExtra/plot/src/*.cpp \
    ./src/OpencvExtra/plot/samples/*.cpp \
    ./src/OpencvExtra/quality/src/*.cpp \
    ./src/OpencvExtra/quality/samples/*.cpp \
    ./src/OpencvExtra/tracking/src/*.cpp \
    ./src/OpencvExtra/tracking/samples/*.cpp
