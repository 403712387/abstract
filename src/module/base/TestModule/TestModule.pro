QT       += core
QT       -= gui
CONFIG += c++11
TARGET = test
TEMPLATE = app
DESTDIR = ../../../bin/abstract
OBJECTS_DIR = ../../../output/TestModule
MOC_DIR = ../../../output/TestModule

INCLUDEPATH += ./src \
            ../FaceTrackModule/src/OpencvExtra/face/samples \
            ../FaceTrackModule/src/OpencvExtra/quality/samples \
            ../FaceTrackModule/src/OpencvExtra/tracking/samples \
            ../FaceTrackModule/src/OpencvExtra/face/include \
            ../FaceTrackModule/src/OpencvExtra/quality/include \
            ../FaceTrackModule/src/OpencvExtra/tracking/include \
            ../FaceTrackModule/src/OpencvExtra/plot/include \
            ../Common/src \
            ../Common/src/log \
            ../Common/src/info \
            ../Common/src/thread \
            ../Common/src/message \
            ../CoreModule/src \
            ../../../../thirdparty/include \
            ../../../../thirdparty/include/Qt \
            ../../../../thirdparty/include/openCV \
            ../../../../thirdparty/include/facedetection
win32 {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule -lFaceTrackModule \
        -L../../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/windows64/facedetection -lfacedetection \
        -L../../../../thirdparty/lib/windows64/openCV -lopencv_core400 -lopencv_video400 -lopencv_videoio400 -lopencv_highgui400 -lopencv_ml400 -lopencv_objdetect400 -lopencv_imgcodecs400 -lopencv_imgproc400 \
        -lopencv_flann400 -lopencv_gapi400 -lopencv_dnn400 -lopencv_photo400 -lopencv_calib3d400
}

unix {
LIBS += -L../../../bin/abstract/lib/module -lCommon -lCoreModule -lFaceTrackModule \
        -L../../../../thirdparty/lib/linux64/Qt -lQt5Core \
        -L../../../../thirdparty/lib/linux64/jsoncpp  -ljsoncpp \
        -L../../../../thirdparty/lib/linux64/log4cplus -llog4cplus \
        -L../../../../thirdparty/lib/linux64/facedetection -lfacedetection \
        -L../../../thirdparty/lib/linux64/gperftools/static -lprofiler -ltcmalloc \
        -L../../../../thirdparty/lib/linux64/openCV -lopencv_core -lopencv_video -lopencv_videoio -lopencv_highgui -lopencv_ml -lopencv_objdetect -lopencv_imgcodecs -lopencv_imgproc \
        -lopencv_flann -lopencv_gapi -lopencv_dnn -lopencv_photo -lopencv_calib3d -lopencv_features2d
}

HEADERS += \
    src/*.h

SOURCES += \
    src/*.cpp
