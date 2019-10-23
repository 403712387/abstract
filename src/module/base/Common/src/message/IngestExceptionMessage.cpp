#include <sstream>
#include "Error.h"
#include "IngestInfo.h"
#include "IngestExceptionMessage.h"
IngestExceptionMessage::IngestExceptionMessage(std::shared_ptr<IngestInfo> ingestInfo, std::shared_ptr<Error> reason, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute)
    :BaseMessage(Ingest_Exception_Message, transType, priority, distribute)
{
    mIngestInfo = ingestInfo;
    mException = reason;
}

std::shared_ptr<IngestInfo> IngestExceptionMessage::getIngestInfo()
{
    return mIngestInfo;
}

// 获取异常原因
std::shared_ptr<Error> IngestExceptionMessage::getExceptionReason()
{
    return mException;
}

std::string IngestExceptionMessage::toString()
{
    std::stringstream buf;
    buf << "ingest info:" << mIngestInfo->toString() << ", exception reason:" << mException->toString() << ", " << BaseMessage::toString();
    return buf.str();
}
