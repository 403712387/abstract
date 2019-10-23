#include "TrackManager.h"
#include "TrackManagerAgent.h"

TrackManager::TrackManager(MessageRoute *messageRoute)
    :BaseProcess(messageRoute, "TrackManager")
{
    mAgent = std::make_shared<TrackManagerAgent>(this);

    subscribeMessage(Track_Start_Message);
    subscribeMessage(Track_Stop_Message);
}

// 初始化模块
bool TrackManager::init()
{
    return true;
}

void TrackManager::beginWork()
{

}

// 卸载模块
void TrackManager::uninit()
{

}

// 处理消息的函数
std::shared_ptr<BaseResponse> TrackManager::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    switch(message->getMessageType())
    {
    case Track_Start_Message:   // 开始跟踪
        break;
    case Track_Stop_Message:    // 停止跟踪
        break;
    }

    return result;
}

// 处理消息的回应
void TrackManager::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{

}
