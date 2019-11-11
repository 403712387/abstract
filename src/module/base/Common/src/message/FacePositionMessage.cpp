#include <sstream>
#include "VideoFrameInfo.h"
#include "TrackCondition.h"
#include "FacePositionMessage.h"

FacePositionMessage::FacePositionMessage(std::shared_ptr<TrackCondition> condition, std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame, MessageTransType transType ,
                          PriorityType priority , MessageDistriubite distribute )
    :BaseMessage(Face_Position_Message, transType, priority, distribute)
{
    mTrackCondition = condition;
    mVideoFrameInfo = videoFrame;
    mFacePositions = faces;
    mIsDetectFrame = isDetectFrame;
}

// 获取跟踪条件
std::shared_ptr<TrackCondition> FacePositionMessage::getTrackCondition()
{
    return mTrackCondition;
}

// 获取视频帧信息
std::shared_ptr<VideoFrameInfo> FacePositionMessage::getVideoFrameInfo()
{
    return mVideoFrameInfo;
}

// 获取人脸位置信息
std::vector<QRect> FacePositionMessage::getFacePositions()
{
    return mFacePositions;
}

// 是否时检测帧
bool FacePositionMessage::isDetectFrame()
{
    return mIsDetectFrame;
}

std::string FacePositionMessage::toString()
{
    std::stringstream buf;
    buf << "track condition:" << mTrackCondition << ", video frame info:" << mVideoFrameInfo->toString() << ", face position:";
    for (QRect rect : mFacePositions)
    {
        buf << Common::getRectString(rect) << ", ";
    }
    buf << ", is detect frame:" << mIsDetectFrame;
    return buf.str();
}
