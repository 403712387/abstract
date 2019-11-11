#ifndef FACE_TRACK_STOP_MESSAGE_H
#define FACE_TRACK_STOP_MESSAGE_H
#include "BaseMessage.h"

/*
 * 停止人脸跟踪
 */
class TrackCondition;
class ABSTRACT_EXPORT FaceTrackStopMessage : public BaseMessage
{
public:
    FaceTrackStopMessage(std::shared_ptr<TrackCondition> condition, MessageTransType transType = Async_Trans_Message,
                              PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取跟踪条件
    std::shared_ptr<TrackCondition> getTrackCondition();

    virtual std::string toString();

private:
    std::shared_ptr<TrackCondition>     mTrackCondition;
};

#endif
