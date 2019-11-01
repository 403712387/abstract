QT       += core
QT       -= gui
CONFIG += c++11 console
TARGET = abstract
TEMPLATE = app
DESTDIR = ../../bin/abstract
OBJECTS_DIR = ../../output/abstract
MOC_DIR = ../../output/abstract
DEFINES += QT_NO_VERSION_TAGGING
#QMAKE_LFLAGS += -fPIC

INCLUDEPATH += \
            ../../module/base/Common/src \
            ../../module/base/Common/src/message \
            ../../module/base/Common/src/perftool \
            ../../module/base/Common/src/info \
            ../../module/base/Common/src/common \
            ../../module/base/CoreModule/src \
            ../../module/base/HttpModule/src \
            ../../module/base/ChooseBestModule/src \
            ../../module/base/FFmpegIngestModule/src \
            ../../module/base/FaceTrackModule/src \
            ../../module/base/ServiceStatusModule/src \
            ../../module/base/ConfigureModule/src \
            ../../module/business/AbstractModule/src \
            ../../../thirdparty/include

win32 {
LIBS += -L../../bin/abstract/lib/module -lCommon -lCoreModule -lHttpModule -lServiceStatusModule -lConfigureModule -lChooseBestModule \
        -lFFmpegIngestModule -lFaceTrackModule -lAbstractModule \
        -L../../../thirdparty/lib/windows64/Qt -lQt5Core \
        -L../../../thirdparty/lib/windows64/jsoncpp  -ljsoncpp
}

unix {
LIBS += -L../../bin/abstract/lib/module -lCommon -lCoreModule -lHttpModule -lServiceStatusModule -lConfigureModule -lChooseBestModule \
        -lFFmpegIngestModule -lFaceTrackModule -lAbstractModule \
        -L../../../thirdparty/lib/linux64/jsoncpp  -ljsoncpp \
        -L../../../thirdparty/lib/linux64/log4cplus -llog4cplus \
        -L../../../thirdparty/lib/linux64/curl -lcurl \
        -L../../../thirdparty/lib/linux64/gperftools/static -lprofiler -ltcmalloc \
        -L../../../thirdparty/lib/linux64/libmicrohttpd  -lmicrohttpd \
        -L../../../thirdparty/lib/linux64/Qt -lQt5Core -licudata -licui18n -licuuc \
        -L../../../thirdparty/lib/linux64/openCV -lopencv_core -lopencv_video -lopencv_videoio -lopencv_highgui -lopencv_ml -lopencv_objdetect -lopencv_imgcodecs -lopencv_imgproc \
        -lopencv_flann -lopencv_gapi -lopencv_dnn -lopencv_photo -lopencv_calib3d -lopencv_features2d \
        -luuid
}

SOURCES += \
    src/*.cpp
