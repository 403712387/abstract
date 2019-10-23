#include <sstream>
#include "AbstractCondition.h"
#include "AbstractStartMessage.h"

AbstractStartMessage::AbstractStartMessage(std::shared_ptr<AbstractCondition> condition, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute)
    :BaseMessage(Abstract_Start_Message, transType, priority, distribute)
{
    mAbstractCondition = condition;
}

// 获取提取的条件
std::shared_ptr<AbstractCondition> AbstractStartMessage::getAbstractCondition()
{
    return mAbstractCondition;
}

std::string AbstractStartMessage::toString()
{
    std::stringstream buf;
    buf << "abstract condition:" << mAbstractCondition->toString() << ", " << BaseMessage::toString();
    return buf.str();
}

/*
 * 开始提取的回应
 */
AbstractStartResponse::AbstractStartResponse(std::string id, std::shared_ptr<BaseMessage> message, std::shared_ptr<Error> error)
    :BaseResponse(message, error)
{
    mAbstractId = id;
}

// 获取提取的ID
std::string AbstractStartResponse::getAbstractId()
{
    return mAbstractId;
}

std::string AbstractStartResponse::toString()
{
    std::stringstream buf;
    buf << "abstract id:" << mAbstractId << ", " << BaseResponse::toString();
    return buf.str();
}
