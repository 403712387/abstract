#include "TrackStopMessage.h"

TrackStopMessage::TrackStopMessage(std::string abstractId, AbstractType type, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute )
    :BaseMessage(Track_Stop_Message, transType, priority, distribute)
{
    mAbstractId = abstractId;
    mAbstractType = type;
}

// 获取提取的ID
std::string TrackStopMessage::getAbstractId()
{
    return mAbstractId;
}

// 获取提取的类型
AbstractType TrackStopMessage::getAbstractType()
{
    return mAbstractType;
}

std::string TrackStopMessage::toString()
{
    std::stringstream buf;
    buf << "id:" << mAbstractId << ", type:" << Common::getAbstraceTypeName(mAbstractType) << ", " << BaseMessage::toString();
    return buf.str();
}
