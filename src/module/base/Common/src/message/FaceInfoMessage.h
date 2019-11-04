#ifndef FACE_INFO_MESSAGE_H
#define FACE_INFO_MESSAGE_H
#include "BaseMessage.h"

// 跟踪出来的人脸信息
class TrackFaceInfo;
class ABSTRACT_EXPORT  FaceInfoMessage : public BaseMessage
{
public:
    FaceInfoMessage(std::shared_ptr<TrackFaceInfo> info, MessageTransType transType = Async_Trans_Message,
                         PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取跟踪出来的人脸信息
    std::shared_ptr<TrackFaceInfo> getFaceInfo();

    virtual std::string toString();

private:
    std::shared_ptr<TrackFaceInfo>      mTrackFaceInfo;
};


#endif
