#include <sstream>
#include "StopAbstract.h"
#include "AbstractStopMessage.h"

AbstractStopMessage::AbstractStopMessage(std::shared_ptr<StopAbstract> stopCondition, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute)
    :BaseMessage(Abstract_Stop_Message, transType, priority, distribute)
{
    mStopAbstractCondition = stopCondition;
}

// 获取停止条件
std::shared_ptr<StopAbstract> AbstractStopMessage::getStopAbstractCondition()
{
    return mStopAbstractCondition;
}

std::string AbstractStopMessage::toString()
{
    std::stringstream buf;
    buf << "stop abstract condition:" << mStopAbstractCondition->toString() << BaseMessage::toString();
    return buf.str();
}
