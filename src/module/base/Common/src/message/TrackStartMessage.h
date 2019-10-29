#ifndef TRACK_START_MESSAGE_H
#define TRACK_START_MESSAGE_H
#include "Common.h"
#include "BaseMessage.h"

/*
 * 开始跟踪的消息
*/
class TrackCondition;
class ABSTRACT_EXPORT  TrackStartMessage : public BaseMessage
{
public:
    TrackStartMessage(std::shared_ptr<TrackCondition> trackInfo, MessageTransType transType = Async_Trans_Message,
                         PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取跟踪的信息
    std::shared_ptr<TrackCondition> getTrackInfo();

    virtual std::string toString();

private:
    std::shared_ptr<TrackCondition>         mTrackInfo;
};

#endif
