#include <QtCore/QtGlobal>
#include <QtCore/QCoreApplication>
#include "Common.h"
#include "MessageRoute.h"
#include "BaseProcess.h"
#include "HttpModule.h"
#include "ConfigureModule.h"
#include "AbstractManager.h"
#include "FaceTrackManager.h"
#include "ServiceStatusModule.h"
#include "FFmpegIngestManager.h"
#include "FacePositionManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    LOG_I("main", "***********Welcome***************");

    // 解析参数
    for (int i = 0; i < argc; ++i)
    {
        std::string param(argv[i]);
        if (param == "-l" && i < (argc - 1))  // 调整日志级别
        {
            common_log::Log_Level logLevel = Common::getLogLevelFromString(argv[i + 1]);
            common_log::setLogLevel(logLevel);
        }
        if (param == "-h" )  // 帮助
        {
            LOG_I("main", "./abstract -l verbose|debug|info|warn|error|fatal")
        }
    }

    // 启动各个模块
    MessageRoute messageRoute;
    ConfigureModule configureManager(&messageRoute);
    HttpModule httpManager(&messageRoute);
    FaceTrackManager trackFaceManager(&messageRoute);
    ServiceStatusModule serviceStatusManager(&messageRoute);
    FFmpegIngestManager ffmageIngestManager(&messageRoute);
    FacePositionManager facePositionManager(&messageRoute);
    AbstractManager abstraceManager(&messageRoute);
    messageRoute.beginWork();

    LOG_I("main", "********************Bye********************");
    return 0;
}
