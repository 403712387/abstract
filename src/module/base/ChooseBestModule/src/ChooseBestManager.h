#ifndef CHOOSE_BEST_MANAGER_H
#define CHOOSE_BEST_MANAGER_H
#include "Common.h"
#include "BaseProcess.h"
/*
 * 从一个人的多张照片中选择N张最好的
*/
class ABSTRACT_EXPORT ChooseBestManager : public BaseProcess
{
public:
    ChooseBestManager(MessageRoute *messageRoute);

protected:
    virtual bool init();
    virtual void beginWork();

    // 卸载模块
    virtual void uninit();

    // 处理消息的函数
    virtual std::shared_ptr<BaseResponse> onProcessMessage(std::shared_ptr<BaseMessage> &message);

    // 处理消息的回应
    virtual void onProcessResponse(std::shared_ptr<BaseResponse> &response);
};

#endif
