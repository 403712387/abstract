#ifndef ABSTRACT_START_MESSAGE_H
#define ABSTRACT_START_MESSAGE_H
#include "BaseMessage.h"

/*
 * 开始提取的消息
*/
class AbstractCondition;
class ABSTRACT_EXPORT  AbstractStartMessage : public BaseMessage
{
public:
    AbstractStartMessage(std::shared_ptr<AbstractCondition> condition, MessageTransType transType = Async_Trans_Message,
                         PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取提取的条件
    std::shared_ptr<AbstractCondition> getAbstractCondition();

    virtual std::string toString();

private:
    std::shared_ptr<AbstractCondition>  mAbstractCondition;
};

/*
 * 开始提取的回应
 */
class ABSTRACT_EXPORT AbstractStartResponse : public BaseResponse
{
public:
    AbstractStartResponse(std::string id, std::shared_ptr<BaseMessage> message, std::shared_ptr<Error> error);

    // 获取提取的ID
    std::string getAbstractId();

    virtual std::string toString();

private:
    std::string     mAbstractId;
};

#endif
