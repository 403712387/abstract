#include <sstream>
#include "TrackFaceInfo.h"
#include "FaceInfoMessage.h"

FaceInfoMessage::FaceInfoMessage(std::shared_ptr<TrackFaceInfo> info, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute)
    :BaseMessage(Face_Info_Message, transType, priority, distribute)
{
    mTrackFaceInfo = info;
}

// 获取跟踪出来的人脸信息
std::shared_ptr<TrackFaceInfo> FaceInfoMessage::getFaceInfo()
{
    return mTrackFaceInfo;
}

std::string FaceInfoMessage::toString()
{
    std::stringstream buf;
    buf << "face info:" << mTrackFaceInfo->toString() << ", " << BaseMessage::toString();
    return buf.str();
}
