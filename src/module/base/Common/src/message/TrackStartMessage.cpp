#include <sstream>
#include "TrackStartMessage.h"

TrackStartMessage::TrackStartMessage(std::string abstractId, AbstractType type, AbstractModel model, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute )
    :BaseMessage(Track_Start_Message, transType, priority, distribute)
{
    mAbstractId = abstractId;
    mAbstractType = type;
    mAbstractModel = model;
}

// 获取提取的ID
std::string TrackStartMessage::getAbstractId()
{
    return mAbstractId;
}

// 获取提取的类型
AbstractType TrackStartMessage::getAbstractType()
{
    return mAbstractType;
}

// 获取提取的模式(质量优先还是实时优先)
AbstractModel TrackStartMessage::getAbstractModel()
{
    return mAbstractModel;
}

std::string TrackStartMessage::toString()
{
    std::stringstream buf;
    buf << "id:" << mAbstractId << ", type:" << Common::getAbstraceTypeName(mAbstractType) << ", " << BaseMessage::toString();
    return buf.str();
}
