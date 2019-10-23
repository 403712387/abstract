#ifndef TRACK_STOP_MESSAGE_H
#define TRACK_STOP_MESSAGE_H
#include "Common.h"
#include "BaseMessage.h"

/*
 * 开始跟踪的消息
*/
class ABSTRACT_EXPORT  TrackStopMessage : public BaseMessage
{
public:
    TrackStopMessage(std::string abstractId, AbstractType type, MessageTransType transType = Async_Trans_Message,
                         PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取提取的ID
    std::string getAbstractId();

    // 获取提取的类型
    AbstractType getAbstractType();

    virtual std::string toString();

private:
    std::string         mAbstractId;
    AbstractType        mAbstractType = Abstract_Face;
};

#endif
