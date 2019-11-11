#ifndef FACE_POSITION_MESSAGE_H
#define FACE_POSITION_MESSAGE_H
#include <vector>
#include <QRect>
#include "BaseMessage.h"

/*
 * 人脸位置信息
 */
class VideoFrameInfo;
class TrackCondition;
class ABSTRACT_EXPORT FacePositionMessage : public BaseMessage
{
public:
    FacePositionMessage(std::shared_ptr<TrackCondition> condition, std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame, MessageTransType transType = Async_Trans_Message,
                              PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取跟踪条件
    std::shared_ptr<TrackCondition> getTrackCondition();

    // 获取视频帧信息
    std::shared_ptr<VideoFrameInfo> getVideoFrameInfo();

    // 获取人脸位置信息
    std::vector<QRect> getFacePositions();

    // 是否时检测帧
    bool isDetectFrame();

    virtual std::string toString();

private:
    std::shared_ptr<TrackCondition>     mTrackCondition;
    std::shared_ptr<VideoFrameInfo>     mVideoFrameInfo;
    std::vector<QRect>                  mFacePositions;
    bool                                mIsDetectFrame = false;
};

#endif
