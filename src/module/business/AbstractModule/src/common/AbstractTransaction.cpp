#include <sstream>
#include "IngestInfo.h"
#include "AbstractTransaction.h"

AbstractTransaction::AbstractTransaction(std::shared_ptr<IngestInfo> ingestInfo)
{
    mIngestInfo = ingestInfo;
}

// 获取事务ID(事务ID与ingest的request相同)
long long AbstractTransaction::getTransactionId()
{
    return mIngestInfo->getRequestId();
}

// 获取ingest信息
std::shared_ptr<IngestInfo> AbstractTransaction::getIngestInfo()
{
    return mIngestInfo;
}

// 结束一个事务
void AbstractTransaction::stopTransaction(std::string reason)
{
    if (mIsStop)
    {
        return;
    }

    mIsStop = true;
    mStopReason = reason;
    mStopTime = QDateTime::currentDateTime();
}

// 判断事务是否结束
bool AbstractTransaction::isTransactionDone()
{
    return mIsStop;
}

// 获取事务结束的时间
QDateTime AbstractTransaction::getStopTime()
{
    return mStopTime;
}

std::string AbstractTransaction::toString()
{
    std::stringstream buf;
    buf << "ingest info:" << mIngestInfo->toString();
    if (mIsStop)
    {
        buf << ", stop time:" << mStopTime.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString() << ", stop reason:" << mStopReason;
    }
    else
    {
        buf << ", is ingestting";
    }
    return buf.str();
}
Json::Value AbstractTransaction::toJson()
{
    Json::Value result;
    result["ingest"] = mIngestInfo->toJson();
    if (mIsStop)
    {
        result["stop"]["reason"] = mStopReason;
        result["stop"]["time"] = mStopTime.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    }
    else
    {
        result["stop"] = mIsStop;
    }
    return result;
}
