#include "TrackCondition.h"
#include "IngestInfo.h"
#include "VideoFrameMessage.h"
#include "FaceTrackManager.h"
#include "TrackStartMessage.h"
#include "TrackStopMessage.h"
#include "IngestExceptionMessage.h"
#include "FaceTrackManagerAgent.h"

FaceTrackManager::FaceTrackManager(MessageRoute *messageRoute)
    :BaseProcess(messageRoute, "FaceTrackManager")
{
    mAgent = std::make_shared<TrackManagerAgent>(this);

    subscribeMessage(Track_Start_Message);
    subscribeMessage(Track_Stop_Message);
    subscribeMessage(Ingest_Exception_Message);
    subscribeMessage(Video_Frame_Message);
}

// 初始化模块
bool FaceTrackManager::init()
{
    return true;
}

void FaceTrackManager::beginWork()
{

}

// 卸载模块
void FaceTrackManager::uninit()
{

}

// 处理消息的函数
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    switch(message->getMessageType())
    {
    case Track_Start_Message:   // 开始跟踪
        result = onProcessStartTrackMessage(message);
        break;
    case Track_Stop_Message:    // 停止跟踪
        result = onProcessStopTrackMessage(message);
        break;
    case Ingest_Exception_Message:  // 拉流的时候出现了异常
        result = onProcessIngestExceptionMessage(message);
        break;
    case Video_Frame_Message:   // 拉取的视频帧信息
        result = onProcessVideoFrameMessage(message);
        break;
    }

    return result;
}

// 处理消息的回应
void FaceTrackManager::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{

}

// 处理开始跟踪的消息
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessStartTrackMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<TrackStartMessage> startMessage = std::dynamic_pointer_cast<TrackStartMessage>(message);
    if (NULL == startMessage.get())
    {
        LOG_E(mClassName, "receive start track message, but message is NULL, message info:" << message->toString());
    }

    if (startMessage->getTrackInfo()->getAbstractType().count(Abstract_Face))
    {
        mAgent->startTrack(startMessage->getTrackInfo());
    }

    return response;
}

// 处理停止跟踪的消息
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessStopTrackMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<TrackStopMessage> stopMessage = std::dynamic_pointer_cast<TrackStopMessage>(message);
    if (NULL == stopMessage.get())
    {
        LOG_E(mClassName, "receive stop track message, but message is NULL, message info:" << message->toString());
    }

    if (Abstract_Face == stopMessage->getAbstractType())
    {
        mAgent->stopTrack(stopMessage->getAbstractId());
    }
    return response;
}

// 处理拉流异常的消息
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessIngestExceptionMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<IngestExceptionMessage> exceptionMessage = std::dynamic_pointer_cast<IngestExceptionMessage>(message);
    if (NULL == exceptionMessage.get())
    {
        LOG_E(mClassName, "receive ingest exception message, but message is NULL, message info:" << message->toString());
    }

    mAgent->pauseTrack(exceptionMessage->getIngestInfo()->getStreamId());
    return response;
}

// 处理视频帧信息
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessVideoFrameMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<VideoFrameMessage> frameMessage = std::dynamic_pointer_cast<VideoFrameMessage>(message);
    if (NULL == frameMessage.get())
    {
        LOG_E(mClassName, "receive video frame message, but message is NULL, message info:" << message->toString());
    }

    mAgent->receiveVideoFrame(frameMessage->getVideoFrameInfo());
    return response;
}
