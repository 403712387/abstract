﻿#ifndef SERVICE_STATUS_MODULE_H
#define SERVICE_STATUS_MODULE_H
#include <atomic>
#include "BaseProcess.h"

/*
 * 服务状态管理，主要管理调试信息，程序运行时的信息等
*/
class ServiceStatusInfo;
class SpeedStatisticList;
class ABSTRACT_EXPORT ServiceStatusModule : public BaseProcess
{
public:
    ServiceStatusModule(MessageRoute *messageRoute);
protected:
    virtual bool init();
    virtual void beginWork();

    // 卸载模块
    virtual void uninit();

    // 处理消息的函数
    virtual std::shared_ptr<BaseResponse> onProcessMessage(std::shared_ptr<BaseMessage> &message);

    // 偷窥消息的处理函数
    virtual bool onForeseeMessage(std::shared_ptr<BaseMessage> &message);

    // 处理消息的回应
    virtual void onProcessResponse(std::shared_ptr<BaseResponse> &response);

private:
    // 处理获取服务状态消息
    std::shared_ptr<BaseResponse> onProcessGetServiceStatusMessage(std::shared_ptr<BaseMessage> &message);
private:

    // 初始化服务的状态信息
    void initServiceStatusInfo();

    // 初始化模块的版本信息
    void initLibraryVersionInfo();

private:
    std::shared_ptr<ServiceStatusInfo>      mServiceStatusInfo;
};
#endif
