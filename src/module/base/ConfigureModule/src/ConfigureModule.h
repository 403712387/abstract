﻿#ifndef CONFIGURE_MODULE_H
#define CONFIGURE_MODULE_H
#include <QtCore/QMap>
#include <atomic>
#include "BaseProcess.h"

/*
 * 配置管理类
*/
class ConfigureJson;
class ConfigureInfo;
class ABSTRACT_EXPORT ConfigureModule : public BaseProcess
{
public:
    ConfigureModule(MessageRoute *messageRoute, std::string configFile = "config/config.json");
protected:
    virtual bool init();
    virtual void beginWork();

    // 卸载模块
    virtual void uninit();

    // 处理消息的函数
    virtual std::shared_ptr<BaseResponse> onProcessMessage(std::shared_ptr<BaseMessage> &message);

    // 处理消息的回应
    virtual void onProcessResponse(std::shared_ptr<BaseResponse> &response);

private:
    // 处理配置消息
    std::shared_ptr<BaseResponse> onProcessConfigMessage(std::shared_ptr<BaseMessage> message);
private:

    // 设置配置信息
    void setConfigureInfo();

    // 发送配置消息
    void sendConfigureMessage();

private:
    std::shared_ptr<ConfigureInfo>      mConfigureInfo;
    std::string                         mConfigFile;

    std::shared_ptr<ConfigureJson>      mConfigureFile;
};
#endif
