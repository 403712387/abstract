#ifndef ABSTRACT_TRANSACTION_H
#define ABSTRACT_TRANSACTION_H
#include <QDateTime>
#include "Common.h"
#include "jsoncpp/json.h"
/*
 * 提取事务，一次提取请求一直到该请求结束，称之为一个事务
 * 一个提取任务中，可能会有多个事务，因为在请求视频的过程中，
 * 可能会因为网络原因，拉取流会终止，此时会终止当前的提取事务，创建一个
 * 新的事务去拉取
 */
class IngestInfo;
class ABSTRACT_EXPORT AbstractTransaction
{
public:
    AbstractTransaction(std::shared_ptr<IngestInfo> ingestInfo);

    // 获取事务ID(事务ID与ingest的request相同)
    long long getTransactionId();

    // 获取ingest信息
    std::shared_ptr<IngestInfo> getIngestInfo();

    // 结束一个事务
    void stopTransaction(std::string reason);

    // 判断事务是否结束
    bool isTransactionDone();

    // 获取事务结束的时间
    QDateTime getStopTime();

    std::string toString();
    Json::Value toJson();

private:
    std::shared_ptr<IngestInfo>     mIngestInfo;
    bool            mIsStop = false;    // 事务是否结束
    std::string     mStopReason;        // 结束原因
    QDateTime       mStopTime;          // 事务结束的时间
};
#endif
