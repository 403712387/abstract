#ifndef ABSTRACT_TASK_H
#define ABSTRACT_TASK_H
#include <QMap>
#include "Common.h"
#include "jsoncpp/json.h"
/*
 * 用户请求提取人脸/人体，会生成一个abstract task,一个abstract task可能会有
 * 一个或者多个abstract transaction
*/
class VideoFormat;
class AbstractCondition;
class AbstractTransaction;
class ABSTRACT_EXPORT AbstractTask
{
public:
    AbstractTask(std::shared_ptr<AbstractCondition> condition, std::string abstractId);

    // 获取abstract condition
    std::shared_ptr<AbstractCondition> getAbstractCondition();

    // 获取提取的ID
    std::string getAbstractId();

    // 视频格式
    void setVideoFormat(std::shared_ptr<VideoFormat> format);
    std::shared_ptr<VideoFormat> getVideoFormat();

    // 添加事务
    void addTransaction(std::shared_ptr<AbstractTransaction> transaction);
    std::vector<std::shared_ptr<AbstractTransaction>> getAllTransaction();
    bool isExistTransaction(long long transactionId);
    std::shared_ptr<AbstractTransaction> getTransaction(long long transactionId);

    // 停止任务
    void stopTask(std::string reason);
    bool isStop();

    std::string toString();
    Json::Value toJson();

private:
    std::string                     mAbstractId;
    std::shared_ptr<AbstractCondition>  mAbstractCondition; //  提取的条件信息
    std::shared_ptr<VideoFormat>    mVideoFormat;       // 视频的格式
    QMap<long long, std::shared_ptr<AbstractTransaction>>   mMapTransaction;    // 事务信息
    bool                            mIsStop = false;
};

#endif
