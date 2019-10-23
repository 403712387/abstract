#include "VideoFormat.h"
#include "AbstractTask.h"
#include "AbstractCondition.h"
#include "AbstractTransaction.h"

AbstractTask::AbstractTask(std::shared_ptr<AbstractCondition> condition, std::string abstractId)
{
    mAbstractCondition = condition;
    mAbstractId = abstractId;
}

// 获取abstract condition
std::shared_ptr<AbstractCondition> AbstractTask::getAbstractCondition()
{
    return mAbstractCondition;
}

// 获取提取的ID
std::string AbstractTask::getAbstractId()
{
    return mAbstractId;
}

// 视频格式
void AbstractTask::setVideoFormat(std::shared_ptr<VideoFormat> format)
{
    mVideoFormat = format;
}

std::shared_ptr<VideoFormat> AbstractTask::getVideoFormat()
{
    return mVideoFormat;
}

// 添加事务
void AbstractTask::addTransaction(std::shared_ptr<AbstractTransaction> transaction)
{
    mMapTransaction.insert(transaction->getTransactionId(), transaction);
}

std::vector<std::shared_ptr<AbstractTransaction>> AbstractTask::getAllTransaction()
{
    std::vector<std::shared_ptr<AbstractTransaction>> transactions;
    QMapIterator<long long, std::shared_ptr<AbstractTransaction>> iterator(mMapTransaction);
    while(iterator.hasNext())
    {
        iterator.next();
        transactions.push_back(iterator.value());
    }
    return transactions;
}

bool AbstractTask::isExistTransaction(long long transactionId)
{
    return mMapTransaction.contains(transactionId);
}

std::shared_ptr<AbstractTransaction> AbstractTask::getTransaction(long long transactionId)
{
    std::shared_ptr<AbstractTransaction> result;
    return mMapTransaction.value(transactionId, result);
}

// 停止任务
void AbstractTask::stopTask(std::string reason)
{

    // 停止所有的拉流事务
    QMapIterator<long long, std::shared_ptr<AbstractTransaction>> iterator(mMapTransaction);
    while(iterator.hasNext())
    {
        iterator.next();
        std::shared_ptr<AbstractTransaction> transaction = iterator.value();
        if (!transaction->isTransactionDone())
        {
            transaction->stopTransaction(reason);
        }
    }
    mIsStop = true;
}

bool AbstractTask::isStop()
{
    return mIsStop;
}

std::string AbstractTask::toString()
{
    std::stringstream buf;
    buf << "abstract condition:" << mAbstractCondition->toString();
    if (NULL != mVideoFormat.get())
    {
        buf << ", video format:" << mVideoFormat->toString();
    }

    buf << ", transaction:";
    QMapIterator<long long, std::shared_ptr<AbstractTransaction>> iterator(mMapTransaction);
    while(iterator.hasNext())
    {
        iterator.next();
        std::shared_ptr<AbstractTransaction> transaction = iterator.value();
        buf << transaction->toString();
    }
    buf <<  ", is stop:" << mIsStop;
    return buf.str();
}

Json::Value AbstractTask::toJson()
{
    Json::Value result;
    result["abstract condition"] = mAbstractCondition->toJson();
    if (NULL != mVideoFormat.get())
    {
        result["video_format"] = mVideoFormat->toJson();
    }

    QMapIterator<long long, std::shared_ptr<AbstractTransaction>> iterator(mMapTransaction);
    int index = 0;
    while(iterator.hasNext())
    {
        iterator.next();
        std::shared_ptr<AbstractTransaction> transaction = iterator.value();
        result["transaction"][index] = transaction->toJson();
        ++index;
    }
    result["is_stop"] = mIsStop;
    return result;
}
