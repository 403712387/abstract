#include <sstream>
#include "TrackCondition.h"
#include "TrackStartMessage.h"

TrackStartMessage::TrackStartMessage(std::shared_ptr<TrackCondition> trackInfo, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute )
    :BaseMessage(Track_Start_Message, transType, priority, distribute)
{
    mTrackInfo = trackInfo;
}

// 获取跟踪的信息
std::shared_ptr<TrackCondition> TrackStartMessage::getTrackInfo()
{
    return mTrackInfo;
}

std::string TrackStartMessage::toString()
{
    std::stringstream buf;
    buf << "track info:" << mTrackInfo->toString() << ", " << BaseMessage::toString();
    return buf.str();
}
