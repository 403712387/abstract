#include "IngestInfo.h"
#include "TrackCondition.h"
#include "FaceTrackManager.h"
#include "FacePositionMessage.h"
#include "FaceTrackStopMessage.h"
#include "FaceTrackManagerAgent.h"

FaceTrackManager::FaceTrackManager(MessageRoute *messageRoute)
    :BaseProcess(messageRoute, "FaceTrackManager")
{
    mAgent = std::make_shared<FaceTrackManagerAgent>(this);

    subscribeMessage(Face_Position_Message);
    subscribeMessage(Face_Track_Stop_Message);
}

// 初始化模块
bool FaceTrackManager::init()
{
    LOG_I(mClassName, "begin init");
    mAgent->init();
    LOG_I(mClassName, "end init");
    return true;
}

void FaceTrackManager::beginWork()
{
    LOG_I(mClassName, "begin work");
}

// 卸载模块
void FaceTrackManager::uninit()
{
    LOG_I(mClassName, "begin uninit");
    mAgent->uninit();
    BaseProcess::uninit();
    LOG_I(mClassName, "end uninit");
}

// 处理消息的函数
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    switch(message->getMessageType())
    {
    case Face_Position_Message:   // 处理人脸位置的信息
        result = onProcessFacePositionMessage(message);
        break;
    case Face_Track_Stop_Message:    // 停止跟踪
        result = onProcessStopTrackMessage(message);
        break;
    }

    return result;
}

// 处理消息的回应
void FaceTrackManager::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{

}

// 消息队列中没有消息的时候调用
void FaceTrackManager::onIdle()
{
    if (NULL != mAgent.get())
    {
        mAgent->checkFaceTrackStatus();
    }
}

// 处理开始跟踪的消息
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessFacePositionMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<FacePositionMessage> positionMessage = std::dynamic_pointer_cast<FacePositionMessage>(message);
    if (NULL == positionMessage.get())
    {
        LOG_E(mClassName, "receive face position message, but message is NULL, message info:" << message->toString());
    }

    mAgent->processFacePosition(positionMessage);
    return response;
}

// 处理停止跟踪的消息
std::shared_ptr<BaseResponse> FaceTrackManager::onProcessStopTrackMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<FaceTrackStopMessage> stopMessage = std::dynamic_pointer_cast<FaceTrackStopMessage>(message);
    if (NULL == stopMessage.get())
    {
        LOG_E(mClassName, "receive stop track face message, but message is NULL, message info:" << message->toString());
    }

    mAgent->stopTrack(stopMessage->getTrackCondition()->getAbstractId());
    return response;
}

