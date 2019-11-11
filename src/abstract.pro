TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += \
        ./module/base/Common/Common.pro \
        ./module/base/CoreModule/CoreModule.pro \
        ./module/base/HttpModule/HttpModule.pro \
        ./module/base/FaceTrackModule/FaceTrackModule.pro \
        ./module/base/FacePositionModule/FacePositionModule.pro \
        ./module/base/TestModule/TestModule.pro \
        ./module/base/FFmpegIngestModule/FFmpegIngestModule.pro \
        ./module/base/ServiceStatusModule/ServiceStatusModule.pro \
        ./module/base/ConfigureModule/ConfigureModule.pro \
        ./module/business/AbstractModule/AbstractModule.pro \
        ./service/abstract/abstract.pro
