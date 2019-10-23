#include "ChooseBestManager.h"

ChooseBestManager::ChooseBestManager(MessageRoute *messageRoute)
    :BaseProcess(messageRoute, "ChooseBestManager")
{
}

// 初始化模块
bool ChooseBestManager::init()
{
    return true;
}

void ChooseBestManager::beginWork()
{

}

// 卸载模块
void ChooseBestManager::uninit()
{

}

// 处理消息的函数
std::shared_ptr<BaseResponse> ChooseBestManager::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> result;
    return result;
}

// 处理消息的回应
void ChooseBestManager::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{

}
