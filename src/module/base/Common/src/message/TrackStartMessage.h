#ifndef TRACK_START_MESSAGE_H
#define TRACK_START_MESSAGE_H
#include "Common.h"
#include "BaseMessage.h"

/*
 * 开始跟踪的消息
*/
class ABSTRACT_EXPORT  TrackStartMessage : public BaseMessage
{
public:
    TrackStartMessage(std::string abstractId, AbstractType type, AbstractModel model, MessageTransType transType = Async_Trans_Message,
                         PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取提取的ID
    std::string getAbstractId();

    // 获取提取的类型
    AbstractType getAbstractType();

    // 获取提取的模式(质量优先还是实时优先)
    AbstractModel getAbstractModel();

    virtual std::string toString();

private:
    std::string         mAbstractId;
    AbstractType        mAbstractType = Abstract_Face;
    AbstractModel       mAbstractModel = Abstract_Quality_First;
};

#endif
