#include <sstream>
#include "FaceTrackStopMessage.h"

FaceTrackStopMessage::FaceTrackStopMessage(std::shared_ptr<TrackCondition> condition, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute)
    :BaseMessage(Face_Track_Stop_Message, transType, priority, distribute)
{
    mTrackCondition = condition;
}

// 获取跟踪条件
std::shared_ptr<TrackCondition> FaceTrackStopMessage::getTrackCondition()
{
    return mTrackCondition;
}

std::string FaceTrackStopMessage::toString()
{
    std::stringstream buf;
    buf << "track condition:" << mTrackCondition << ", " << BaseMessage::toString();
    return buf.str();
}
