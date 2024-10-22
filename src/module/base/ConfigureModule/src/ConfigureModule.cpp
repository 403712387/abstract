﻿#include <stdio.h>
#include <sstream>
#include "ConfigureInfo.h"
#include "ConfigureJson.h"
#include "ConfigureModule.h"
#include "ConfigureInfoMessage.h"
ConfigureModule::ConfigureModule(MessageRoute *messageRoute, std::string configFile)
    :BaseProcess(messageRoute, "ConfigureModule")
{
    mConfigFile = configFile;
    mConfigureInfo = std::make_shared<ConfigureInfo>();

    // 订阅消息
    subscribeMessage(Config_Message);
}

bool ConfigureModule::init()
{
    LOG_I(mClassName, "begin init");
    mConfigureFile = std::make_shared<ConfigureJson>(mConfigFile);
    LOG_I(mClassName, "configure file:" << mConfigFile << "configure info:" << mConfigureFile->toString());

    //  设置配置信息
    setConfigureInfo();

    // 发送配置消息
    sendConfigureMessage();

    LOG_I(mClassName, "end module");
    return true;
}

void ConfigureModule::beginWork()
{
    LOG_I(mClassName, "begin work, module " << getModuleName());
}

// 卸载模块
void ConfigureModule::uninit()
{
    LOG_I(mClassName, "begin uninit module ");
    mConfigureFile.reset();
    BaseProcess::uninit();
    LOG_I(mClassName, "end uninit module ");
}

// 处理消息的函数
std::shared_ptr<BaseResponse> ConfigureModule::onProcessMessage(std::shared_ptr<BaseMessage> &message)
{
    std::shared_ptr<BaseResponse> response;
    switch(message->getMessageType())
    {
    case Config_Message:        // 配置信息
        response = onProcessConfigMessage(message);
    }

    return response;
}

// 处理消息的回应
void ConfigureModule::onProcessResponse(std::shared_ptr<BaseResponse> &response)
{

}

// 设置配置信息
void ConfigureModule::setConfigureInfo()
{
    if (NULL == mConfigureFile.get() || NULL == mConfigureInfo.get())
    {
        return;
    }

    // 设置监听的端口
    mConfigureInfo->setHttpPort(mConfigureFile->getHttpPort(1218));

    // 设置日志级别
    mConfigureInfo->setLogLevel(common_log::getLogLevel());
}

// 发送配置消息
void ConfigureModule::sendConfigureMessage()
{
    if (NULL == mConfigureInfo.get())
    {
        return;
    }

    std::shared_ptr<ConfigureInfoMessage> message = std::make_shared<ConfigureInfoMessage>(mConfigureInfo, Sync_Trans_Message);
    sendMessage(message);
}

// 处理配置消息
std::shared_ptr<BaseResponse> ConfigureModule::onProcessConfigMessage(std::shared_ptr<BaseMessage> message)
{
    std::shared_ptr<BaseResponse> response;
    std::shared_ptr<ConfigureInfoMessage> configureMessage = std::dynamic_pointer_cast<ConfigureInfoMessage>(message);
    mConfigureInfo = configureMessage->getConfigureInfo();

    // 更新配置文件
    mConfigureFile->setHttpPort(mConfigureInfo->getHttpPort());

    return response;
}
