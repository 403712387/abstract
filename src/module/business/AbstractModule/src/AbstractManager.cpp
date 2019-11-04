#include "AbstractManager.h"
#include "TrackStopMessage.h"
#include "TrackStartMessage.h"
#include "StopIngestMessage.h"
#include "AbstractStopMessage.h"
#include "FFmpegIngestMessage.h"
#include "AbstractManagerAgent.h"
#include "AbstractStartMessage.h"
#include "IngestExceptionMessage.h"

AbstractManager::AbstractManager(MessageRoute *messageRoute)
    :BaseProcess(messageRoute, "AbstractManager")
{
    mAgent = std::make_shared<AbstractManagerAgent>(this);

    // 订阅消息
    subscribeMessage(Abstract_Stop_Message);
    subscribeMessage(Abstract_Start_Message);
    subscribeMessage(Ingest_Exception_Message);
}

// 初始化模块
bool AbstractManager::init()
{
    LOG_I(mClassName, "begin init");
    mAgent->init();
    LOG_I(mClassName, "end init");
    return true;
}

void AbstractManager::beginWork()
{
    LOG_I(mClassName, "begin work");
}

// 卸载模块
void AbstractManager::uninit()
{
    LOG_I(mClassName, "begin uninit");
    mAgent->uninit();
    BaseProcess::uninit();
    LOG_I(mClassName, "end uninit");
}

// 处理消息的函数
std::shared_ptr<BaseResponse> AbstractManager::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    switch(message->getMessageType())
    {
    case Abstract_Start_Message:    // 开始提取人脸/人体
        result = onProcessAbstractStartMessage(message);
        break;
    case Abstract_Stop_Message:     // 停止提取人脸/人体
        result = onProcessAbstractStopMessage(message);
        break;
    case Ingest_Exception_Message:  // 拉流异常
        result = onProcessIngestExceptionMessage(message);
        break;
    }

    return result;
}

// 处理消息的回应
void AbstractManager::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{
    switch(response->getMessage()->getMessageType())
    {
    case FFmpeg_Ingest_Message:     // ffmpeg 拉流的消息
        onProcessFFmepgIngestResponse(response);
        break;
    }
}

// 处理开始提取人脸/人体的消息
std::shared_ptr<BaseResponse> AbstractManager::onProcessAbstractStartMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    std::shared_ptr<AbstractStartMessage> startMessage = std::dynamic_pointer_cast<AbstractStartMessage>(message);
    if (NULL == startMessage.get())
    {
        LOG_E(mClassName, "receive start abstrace message, but message is NULL, message info:" << message->toString());
        return result;
    }

    std::pair<std::string, std::shared_ptr<Error>> ret = mAgent->startAbstract(startMessage->getAbstractCondition());
    result = std::make_shared<AbstractStartResponse>(ret.first, message, ret.second);
    return result;
}

// 处理停止提取人脸/人体
std::shared_ptr<BaseResponse> AbstractManager::onProcessAbstractStopMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    std::shared_ptr<AbstractStopMessage> stopMessage = std::dynamic_pointer_cast<AbstractStopMessage>(message);
    if (NULL == stopMessage.get())
    {
        LOG_E(mClassName, "receive stop abstrace message, but message is NULL, message info:" << message->toString());
        return result;
    }

    mAgent->stopAbstract(stopMessage->getStopAbstractCondition());
    return result;
}

// 处理拉流异常的消息
std::shared_ptr<BaseResponse> AbstractManager::onProcessIngestExceptionMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    std::shared_ptr<IngestExceptionMessage> exceptionMessage = std::dynamic_pointer_cast<IngestExceptionMessage>(message);
    if (NULL == exceptionMessage.get())
    {
        LOG_E(mClassName, "receive ingest exception message, but message is NULL, message info:" << message->toString());
        return result;
    }

    mAgent->ingestException(exceptionMessage->getIngestInfo(), exceptionMessage->getExceptionReason());
    return result;
}

// 处理ffmpeg拉流的回应
void AbstractManager::onProcessFFmepgIngestResponse(std::shared_ptr<BaseResponse> &response)
{
    std::shared_ptr<FFmpegIngestResponse> ingestResponse = std::dynamic_pointer_cast<FFmpegIngestResponse>(response);
    if (NULL == ingestResponse.get())
    {
        LOG_E(mClassName, "receive ffmpeg ingest response, but response is NULL, response info:" << response->toString());
        return ;
    }

    std::shared_ptr<FFmpegIngestMessage> ingestMessage = std::dynamic_pointer_cast<FFmpegIngestMessage>(ingestResponse->getMessage());

    // 如果是同步消息，则忽略
    if (Sync_Trans_Message == ingestMessage->getTransType())
    {
        return;
    }

    mAgent->processFFmpegIngestResponse(ingestMessage->getFFmpegIngestInfo(), ingestResponse->getVideoFormat(), ingestResponse->getError());
}

// 停止拉流
bool AbstractManager::sendStopIngestStreamMessage(std::shared_ptr<IngestInfo> ingest)
{
    std::shared_ptr<StopIngestMessage> message = std::make_shared<StopIngestMessage>(ingest);
    sendMessage(message);
    return true;
}

// 发送开始跟踪消息
void AbstractManager::sendStartTrackMessage(std::shared_ptr<TrackCondition> trackInfo)
{
    std::shared_ptr<TrackStartMessage> message = std::make_shared<TrackStartMessage>(trackInfo);
    sendMessage(message);
}

// 发送停止跟踪消息
void AbstractManager::sendStopTrackMessage(std::string abstractId, AbstractType type)
{
    std::shared_ptr<TrackStopMessage> message = std::make_shared<TrackStopMessage>(abstractId, type);
    sendMessage(message);
}
