#ifndef ABSTRACT_STOP_MESSAGE_H
#define ABSTRACT_STOP_MESSAGE_H
#include "BaseMessage.h"

/*
 * 停止提取的消息
*/
class StopAbstract;
class ABSTRACT_EXPORT  AbstractStopMessage : public BaseMessage
{
public:
    AbstractStopMessage(std::shared_ptr<StopAbstract> stopCondition, MessageTransType transType = Async_Trans_Message,
                         PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取停止条件
    std::shared_ptr<StopAbstract> getStopAbstractCondition();

    virtual std::string toString();

private:
    std::shared_ptr<StopAbstract> mStopAbstractCondition;
};
#endif
