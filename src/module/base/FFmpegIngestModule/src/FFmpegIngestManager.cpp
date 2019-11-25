#include "VideoFormat.h"
#include "ConfigureInfo.h"
#include "StopIngestMessage.h"
#include "FFmpegIngestMessage.h"
#include "FFmpegIngestManager.h"
#include "VideoFrameMessage.h"
#include "ConfigureInfoMessage.h"
#include "IngestExceptionMessage.h"
#include "FFmpegIngestManagerAgent.h"

FFmpegIngestManager::FFmpegIngestManager(MessageRoute *messageRoute)
    :BaseProcess(messageRoute, "FFmpegIngestManager")
{
    mAgent = std::make_shared<FFmpegIngestManagerAgent>(this);

    // 订阅消息
    subscribeMessage(FFmpeg_Ingest_Message);
    subscribeMessage(Stop_Ingest_Message);
}

bool FFmpegIngestManager::init()
{
    LOG_I(mClassName, "begin init");
    mAgent->init();
    LOG_I(mClassName, "end init");
    return true;
}

void FFmpegIngestManager::beginWork()
{
    LOG_I(mClassName, "begin work");

    LOG_I(mClassName, "end begin work");
}

// 卸载模块
void FFmpegIngestManager::uninit()
{
    LOG_I(mClassName, "begin uninit");
    mAgent->uninit();
    BaseProcess::uninit();
    LOG_I(mClassName, "end uninit");
}

// 处理消息的函数
std::shared_ptr<BaseResponse> FFmpegIngestManager::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    switch (message->getMessageType())
    {
    case FFmpeg_Ingest_Message:     // 拉取视频流
        result = onProcessFFmpegIngestMessage(message);
        break;
    case Stop_Ingest_Message:       // 停止拉取流
        result = onProcessStopIngestMessage(message);
        break;
    case Config_Message:    // 配置信息
        result = onProcessConfigureMessage(message);
        break;
    }

    return result;
}

// 处理消息的回应
void FFmpegIngestManager::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{

}

// 处理拉取视频流的请求
std::shared_ptr<BaseResponse> FFmpegIngestManager::onProcessFFmpegIngestMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    std::shared_ptr<FFmpegIngestMessage> ingestMessage = std::dynamic_pointer_cast<FFmpegIngestMessage>(message);
    if (NULL == ingestMessage)
    {
        LOG_E(mClassName, "receive ffmpeg ingest message, but message is NULL, message info:" << message->toString());
        return result;
    }

    // 处理拉流的请求
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> ret = mAgent->processIngest(ingestMessage->getFFmpegIngestInfo());
    std::shared_ptr<FFmpegIngestResponse> response = std::make_shared<FFmpegIngestResponse>(ret.first, message, ret.second);
    return response;
}

// 处理停止拉取视频流的请求
std::shared_ptr<BaseResponse> FFmpegIngestManager::onProcessStopIngestMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    std::shared_ptr<StopIngestMessage> stopMessage = std::dynamic_pointer_cast<StopIngestMessage>(message);
    if (NULL == stopMessage)
    {
        LOG_E(mClassName, "receive stop ingest message, but message is NULL, message info:" << message->toString());
        return result;
    }

    // 停止拉取流
    mAgent->processStopIngest(stopMessage->getIngestInfo());
    return result;
}

// 下发的配置信息
std::shared_ptr<BaseResponse> FFmpegIngestManager::onProcessConfigureMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<ConfigureInfoMessage> configureMessage = std::dynamic_pointer_cast<ConfigureInfoMessage>(message);
    if (NULL == configureMessage.get())
    {
        LOG_E(mClassName, "receive configure info message, but message is NULL, message info:" << message->toString());
        return response;
    }

    // 设置ffmpeg的日志级别
    mAgent->setFFmpegLogLevel(configureMessage->getConfigureInfo()->getLogLevel());
    return response;
}

// 发送视频帧
void FFmpegIngestManager::sendVideoFrame(std::shared_ptr<VideoFrameInfo> frame)
{
    std::shared_ptr<VideoFrameMessage> message = std::make_shared<VideoFrameMessage>(frame);
    sendMessage(message);
}

// 拉流异常或停止，发送消息通知业务模块
void FFmpegIngestManager::sendIngestStopMessage(std::shared_ptr<IngestInfo> ingestInfo, StopIngestReason reason)
{
    std::shared_ptr<Error> error = Common::getError(Common::getStopIngestReasonString(reason), reason);
    std::shared_ptr<IngestExceptionMessage> message = std::make_shared<IngestExceptionMessage>(ingestInfo, error);
    sendMessage(message);
}
