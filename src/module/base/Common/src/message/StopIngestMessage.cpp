#include <sstream>
#include "StopIngestMessage.h"

StopIngestMessage::StopIngestMessage(std::shared_ptr<IngestInfo> ingest, MessageTransType transType ,PriorityType priority , MessageDistriubite distribute )
    :BaseMessage(Stop_Ingest_Message, transType, priority, distribute)
{
    mIngestInfo = ingest;
}

// 获取拉流信息
std::shared_ptr<IngestInfo> StopIngestMessage::getIngestInfo()
{
    return mIngestInfo;
}

std::string StopIngestMessage::toString()
{
    std::stringstream buf;
    buf << "ingest info:" << mIngestInfo << ", " << BaseMessage::toString();
    return buf.str();
}
