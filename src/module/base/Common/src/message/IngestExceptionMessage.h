#ifndef INGEST_EXCEPTION_MESSAGE_H
#define INGEST_EXCEPTION_MESSAGE_H
#include "BaseMessage.h"
/*
 * 拉取流异常
*/
class Error;
class IngestInfo;
class ABSTRACT_EXPORT IngestExceptionMessage : public BaseMessage
{
public:
public:
    IngestExceptionMessage(std::shared_ptr<IngestInfo> ingestInfo, std::shared_ptr<Error> reason, MessageTransType transType = Async_Trans_Message,
                       PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    std::shared_ptr<IngestInfo> getIngestInfo();

    // 获取异常原因
    std::shared_ptr<Error> getExceptionReason();

    virtual std::string toString();

private:
    std::shared_ptr<IngestInfo>      mIngestInfo;
    std::shared_ptr<Error>           mException;
};

#endif
