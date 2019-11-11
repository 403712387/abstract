#include <QDir>
#include <sstream>
#include <algorithm>
#include <QFileInfo>
#include <QCoreApplication>
#include "IngestInfo.h"
#include "FrameRatio.h"
#include "VideoFormat.h"
#include "AbstractTask.h"
#include "StopAbstract.h"
#include "TrackFaceInfo.h"
#include "TrackCondition.h"
#include "AbstractCommon.h"
#include "AbstractManager.h"
#include "AbstractCondition.h"
#include "FFmpegIngestMessage.h"
#include "AbstractTransaction.h"
#include "AbstractManagerAgent.h"

AbstractManagerAgent::AbstractManagerAgent(AbstractManager *manager)
{
    mManager = manager;
}

// 初始化
bool AbstractManagerAgent::init()
{
    return true;
}

// 反初始化
bool AbstractManagerAgent::uninit()
{
    // 删除所有的任务
    std::vector<std::shared_ptr<AbstractTask>> allTask = getAllTask();
    for (std::shared_ptr<AbstractTask> task : allTask)
    {
        std::shared_ptr<StopAbstract> condition = std::make_shared<StopAbstract>(task->getAbstractId(), task->getAbstractCondition()->getAbstractType());
        stopAbstract(condition);
    }
    return true;
}

// 开始提取人脸/人体信息
std::pair<std::string, std::shared_ptr<Error>> AbstractManagerAgent::startAbstract(std::shared_ptr<AbstractCondition> condition)
{
    std::pair<std::string, std::shared_ptr<Error>> result;

    // 判断是否已经存在对应的任务
    std::string id = getAbstractId(condition->getStreamUrl());
    std::shared_ptr<AbstractTask> task = getTask(id);
    if (NULL == task.get())
    {
        // 新增加一个任务
        result = processAddTask(id, condition);
    }
    else
    {
        // 更新一个任务
        std::set<AbstractType> types = condition->getAbstractType();
        for (AbstractType type : task->getAbstractCondition()->getAbstractType())
        {
            types.insert(type);
        }
        result = processUpdateTask(task, types);
    }

    return result;
}

// 停止提取人脸/人体信息
bool AbstractManagerAgent::stopAbstract(std::shared_ptr<StopAbstract> condition)
{
    // 判断是否有对应的任务
    std::shared_ptr<AbstractTask> task = getTask(condition->getAbstractId());
    if (NULL == task.get())
    {
        LOG_E(mClassName, "stop abstract fail, not find task, stop condition:" << condition->toString());
        return false;
    }

    // 更新提取类型
    std::set<AbstractType> types = task->getAbstractCondition()->getAbstractType();
    for (AbstractType type : condition->getAbstractType())
    {
        auto iter = types.find(type);
        if (types.end() != iter)
        {
            types.erase(iter);
        }
    }

    // 如果提取类型为空，则直接删除任务
    if (types.empty())
    {
        processRemoveTask(condition->getAbstractId());
    }
    else
    {
        // 如果提取类型不为空，则更新任务
        processUpdateTask(task, types);
    }

    return true;
}

// 拉流异常
bool AbstractManagerAgent::ingestException(std::shared_ptr<IngestInfo> ingestInfo, std::shared_ptr<Error> reason)
{
    // 如果不存在对应的任务，则直接返回
    std::shared_ptr<AbstractTask> task = getTask(ingestInfo->getStreamId());
    if (NULL == task.get())
    {
        LOG_E(mClassName, "process ingest exception fail, not find task, ingest info:" << ingestInfo->toString());
        return false;
    }

    // 停止拉流
    task->getTransaction(ingestInfo->getRequestId())->stopTransaction(reason->getErrorReason());
    mManager->sendStopIngestStreamMessage(ingestInfo);
    if (Common::isLocalFile(ingestInfo->getStreamUrl()) && Stop_End_Of_File == reason->getErrorCode())
    {
        // 如果为本地文件，并且到了文件尾，则直接删除任务
        removeTask(task->getAbstractId());
    }
    else
    {
        // 继续拉流分析
        std::shared_ptr<IngestInfo> ingest = std::make_shared<IngestInfo>(task->getAbstractId(), task->getAbstractCondition()->getStreamUrl());
        std::shared_ptr<AbstractTransaction> transaction = std::make_shared<AbstractTransaction>(ingest);
        task->addTransaction(transaction);
        ffmpegIngestStream(ingest, Async_Trans_Message);
    }

    return true;
}

// 接收到跟踪出来的人脸信息
void AbstractManagerAgent::receiveFaceInfo(std::shared_ptr<TrackFaceInfo> faceInfo)
{
    // 暂时把跟踪出来的人脸保存到本地
    QString path = "/track/" + QString::fromStdString(faceInfo->getAbstractId());

    // 创建目录
    QString currentPath = QCoreApplication::applicationDirPath();
    QString imagePath = QDir::toNativeSeparators(currentPath + path);
    Common::createPath(imagePath.toStdString());

    // 写背景大图文件
    static int index = 0;
    std::stringstream backgroundImageName;
    backgroundImageName << faceInfo->getFaceId() << "_" << index++ << ".jpg";
    QString backgroundImagePath = QDir::toNativeSeparators(imagePath + "/" + QString::fromStdString(backgroundImageName.str()));
    std::string imageData = Common::getImageData(faceInfo->getImageMat().get(), 85);
    Common::writeFile(backgroundImagePath.toStdString(), imageData);

    // 写人脸小图
    std::stringstream faceImageName;
    faceImageName << faceInfo->getFaceId() << "_" << index++ << "face.jpg";
    QString faceImagePath = QDir::toNativeSeparators(imagePath + "/" + QString::fromStdString(faceImageName.str()));
    std::shared_ptr<cv::Mat> faceMat = Common::getSubMat(faceInfo->getImageMat().get(), faceInfo->getFaceRect());
    std::string faceData = Common::getImageData(faceMat.get(), 85);
    Common::writeFile(faceImagePath.toStdString(), faceData);
}

// ffmpeg拉流的回应
void AbstractManagerAgent::processFFmpegIngestResponse(std::shared_ptr<IngestInfo> ingestInfo, std::shared_ptr<VideoFormat> format, std::shared_ptr<Error> failReason)
{
    // 如果不存在对应的任务，则直接返回
    std::shared_ptr<AbstractTask> task = getTask(ingestInfo->getStreamId());
    if (NULL == task.get())
    {
        LOG_E(mClassName, "process ffmpeg ingest response fail, not find task, ingest info:" << ingestInfo->toString());
        return;
    }

    if (NULL != failReason.get())
    {
        // 拉流失败
        ingestException(ingestInfo, failReason);
    }
}

// 处理添加任务
std::pair<std::string, std::shared_ptr<Error>> AbstractManagerAgent::processAddTask(std::string id, std::shared_ptr<AbstractCondition> condition)
{
    std::pair<std::string, std::shared_ptr<Error>> result;

    // 拉取视频流
    std::shared_ptr<IngestInfo> ingest = std::make_shared<IngestInfo>(id, condition->getStreamUrl());
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> ret = ffmpegIngestStream(ingest, Sync_Trans_Message);
    if (NULL != ret.second.get())
    {
        LOG_E(mClassName, "ingest stream fail, error reason:" << ret.second->getErrorReason());
        result.second = ret.second;
        return result;
    }

    // 发送开始跟踪的消息
    sendStartTrackMessage(id, condition->getAbstractType(), condition->getAbstractModel(), ret.first->getFrameRatio()->getFrameRatio(), condition->getAbstractCount());

    // 创建任务
    std::shared_ptr<AbstractTask> task = std::make_shared<AbstractTask>(condition, id);
    std::shared_ptr<AbstractTransaction> transaction = std::make_shared<AbstractTransaction>(ingest);
    task->setVideoFormat(ret.first);
    task->addTransaction(transaction);
    addTask(task);

    result.first = id;
    return result;
}

// 处理删除任务
bool AbstractManagerAgent::processRemoveTask(std::string id)
{
    std::shared_ptr<AbstractTask> task = getTask(id);
    if (NULL == task.get())
    {
        LOG_E(mClassName, "remove task fail, not find task, abstract id:" << id);
        return false;
    }

    // 停止所有的事务
    std::vector<std::shared_ptr<AbstractTransaction>> transactions = task->getAllTransaction();
    for (std::shared_ptr<AbstractTransaction> transaction : transactions)
    {
        if (transaction->isTransactionDone())
        {
            continue;
        }

        mManager->sendStopIngestStreamMessage(transaction->getIngestInfo());
        transaction->stopTransaction("task stop");
    }
    task->stopTask("task stop");

    // 发送停止跟踪的消息
    sendStopTrackMessage(id, task->getAbstractCondition()->getAbstractType());

    // 删除任务
    removeTask(id);
    return true;
}

// 处理更新任务
std::pair<std::string, std::shared_ptr<Error>> AbstractManagerAgent::processUpdateTask(std::shared_ptr<AbstractTask> task, std::set<AbstractType> types)
{
    std::pair<std::string, std::shared_ptr<Error>> result;
    result.first = task->getAbstractId();
    std::set<AbstractType> oldTypes = task->getAbstractCondition()->getAbstractType();

    // 找出新增的跟踪类型和删除的跟踪类型
    std::set<AbstractType> removeTypes;
    std::set_difference(oldTypes.begin(), oldTypes.end(), types.begin(), types.end(), std::inserter(removeTypes,removeTypes.begin()));
    std::set<AbstractType> addTypes;
    std::set_difference(types.begin(), types.end(), oldTypes.begin(), oldTypes.end(), std::inserter(addTypes,addTypes.begin()));

    // 发送开始跟踪和停止跟踪的消息
    sendStartTrackMessage(task->getAbstractId(), addTypes, task->getAbstractCondition()->getAbstractModel(), task->getVideoFormat()->getFrameRatio()->getFrameRatio(), task->getAbstractCondition()->getAbstractCount());
    sendStopTrackMessage(task->getAbstractId(), removeTypes);

    // 更新提取的类型
    task->getAbstractCondition()->updateAbstractType(types);
    return result;
}

// 添加任务
void AbstractManagerAgent::addTask(std::shared_ptr<AbstractTask> &task)
{
    std::unique_lock<std::mutex> autoLock(mMapTaskLock);
    mMapTask.insert(task->getAbstractId(), task);
    LOG_I(mClassName, "add abstract task, task info:" << task->toString());
}

// 获取任务
std::shared_ptr<AbstractTask> AbstractManagerAgent::getTask(std::string id)
{
    std::shared_ptr<AbstractTask> result;
    std::unique_lock<std::mutex> autoLock(mMapTaskLock);
    result = mMapTask.value(id, result);
    return result;
}

// 是否存在任务
bool AbstractManagerAgent::isExistTask(std::string id)
{
    std::unique_lock<std::mutex> autoLock(mMapTaskLock);
    return mMapTask.count(id);
}

// 删除任务
bool AbstractManagerAgent::removeTask(std::string id)
{
    std::unique_lock<std::mutex> autoLock(mMapTaskLock);
    LOG_I(mClassName, "remove task, task id:"  << id);
    return mMapTask.remove(id);
}

// 获取所有的任务
std::vector<std::shared_ptr<AbstractTask>> AbstractManagerAgent::getAllTask()
{
    std::vector<std::shared_ptr<AbstractTask>> result;
    std::unique_lock<std::mutex> autoLock(mMapTaskLock);
    QMapIterator<std::string, std::shared_ptr<AbstractTask>> iter(mMapTask);
    while(iter.hasNext())
    {
        iter.next();
        result.push_back(iter.value());
    }
    return result;
}

// ffmpeg拉取流
std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> AbstractManagerAgent::ffmpegIngestStream(std::shared_ptr<IngestInfo> ingest, MessageTransType transType)
{
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> result;

    // 发送消息
    std::shared_ptr<FFmpegIngestMessage> message = std::make_shared<FFmpegIngestMessage>(ingest, transType);
    std::shared_ptr<BaseResponse> response = mManager->sendMessage(message);

    // 如果发送的是异步消息，则直接返回
    if (Async_Trans_Message == transType)
    {
        return result;
    }

    // 如果时同步消息，处理消息的回应
    std::shared_ptr<FFmpegIngestResponse> ingestResponse = std::dynamic_pointer_cast<FFmpegIngestResponse>(response);
    if (NULL == ingestResponse.get())
    {
        result.second = Common::getError("ingest message response is NULL");
        LOG_E(mClassName, "receive ffmpeg ingest response, but response is NULL");
        return result;
    }

    // 解析回应
    result.first = ingestResponse->getVideoFormat();
    result.second = ingestResponse->getError();
    return result;
}

// 发送开始跟踪消息
void AbstractManagerAgent::sendStartTrackMessage(std::string abstractId, std::set<AbstractType> types, AbstractModel model, int frameRatio, int abstractCount)
{
    std::shared_ptr<TrackCondition> trackInfo = std::make_shared<TrackCondition>(abstractId, types, model, frameRatio, abstractCount);
    mManager->sendStartTrackMessage(trackInfo);
}

// 发送停止跟踪消息
void AbstractManagerAgent::sendStopTrackMessage(std::string abstractId, std::set<AbstractType> types)
{
    for (AbstractType type : types)
    {
        mManager->sendStopTrackMessage(abstractId, type);
    }
}
