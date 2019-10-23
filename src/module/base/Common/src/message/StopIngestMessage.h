#ifndef STOP_INGEST_MESSAGE_H
#define STOP_INGEST_MESSAGE_H
#include "BaseMessage.h"

// 停止拉流
class IngestInfo;
class ABSTRACT_EXPORT StopIngestMessage  : public BaseMessage
{
public:
    StopIngestMessage(std::shared_ptr<IngestInfo> ingest, MessageTransType transType = Async_Trans_Message,
                      PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取拉流信息
    std::shared_ptr<IngestInfo> getIngestInfo();

    virtual std::string toString();
private:
    std::shared_ptr<IngestInfo>     mIngestInfo;

};

#endif
