#include "IngestInfo.h"
#include "TrackCondition.h"
#include "FaceInfoMessage.h"
#include "VideoFrameInfo.h"
#include "FaceTrackManager.h"
#include "FaceTrackProcessor.h"
#include "FacePositionMessage.h"
#include "FaceTrackManagerAgent.h"
#include "IngestExceptionMessage.h"
#include "opencv2/opencv.hpp"

FaceTrackManagerAgent::FaceTrackManagerAgent(FaceTrackManager *manager)
{
    mManager = manager;
}

// 初始化
bool FaceTrackManagerAgent::init()
{
#if 1
    bool isUse = cv::useOptimized();
    LOG_I(mClassName, isUse ? "use optimization" : "not use optimization");
    LOG_I(mClassName, "getNumThreads: " << cv::getNumThreads() << ", getThreadNum: " << cv::getThreadNum());
    cv::setNumThreads(0);
    LOG_I(mClassName, "getNumThreads: " << cv::getNumThreads() << ", getThreadNum: " << cv::getThreadNum());
    LOG_I(mClassName, "cpu feature:" << cv::getCPUFeaturesLine().c_str());
    LOG_I(mClassName, "build inforamtion:" << cv::getBuildInformation().c_str());
#endif
    return true;
}

// 反初始化
bool FaceTrackManagerAgent::uninit()
{
    removeAllTrackProcessor();
    return true;
}

// 停止跟踪
bool FaceTrackManagerAgent::stopTrack(std::string abstractId)
{
    QMap<long long, std::shared_ptr<FaceTrackProcessor>> processors = getTrackProcessByAbstraceId(abstractId);
    QMapIterator<long long, std::shared_ptr<FaceTrackProcessor>> iter(processors);
    while(iter.hasNext())
    {
        iter.next();
        std::shared_ptr<FaceTrackProcessor> processor = iter.value();

        // 停止跟踪
        processor->uninit();
    }

    // 删除processor
    removeTrackProcessor(abstractId, 0);
    return true;
}

// 接收到视频帧
bool FaceTrackManagerAgent::processFacePosition(std::shared_ptr<FacePositionMessage> message)
{
    // 获取本路流的所有人脸
    std::vector<QRect> facePositions = message->getFacePositions();
    QMap<long long, std::shared_ptr<FaceTrackProcessor>> allProcessor = getTrackProcessByAbstraceId(message->getTrackCondition()->getAbstractId());

    // 更新人脸在新的视频帧中的位置
    for (std::shared_ptr<FaceTrackProcessor> processor : allProcessor)
    {
        processor->receiveFacePositionMessage(message);

        if (!message->isDetectFrame())
        {
            continue;
        }

        // 如果是检测帧,则找出跟踪的出来的人脸和检测出来的哪个人脸重合(这样做的目的是为了查找出视频中新出现的人脸)
        int index = rectInAllFacePositions(facePositions, processor->getCurrentFacePosition());
        if (index >= 0)
        {
            facePositions.erase(facePositions.begin() + index);
        }
    }

    if (!message->isDetectFrame())
    {
        return true;
    }

    // 检查跟踪的状态
    checkFaceTrackStatus(allProcessor);

    // 新增加的人脸
    for (QRect &position : facePositions)
    {
        std::shared_ptr<FaceTrackProcessor> processor = std::make_shared<FaceTrackProcessor>(this);
        processor->init(message->getTrackCondition(), message->getVideoFrameInfo(), position);
        addTrackProcessor(processor);
    }

    return true;
}

// 发送跟踪出来的人脸
void FaceTrackManagerAgent::sendTrackFace(std::shared_ptr<TrackFaceInfo> face)
{
    std::shared_ptr<FaceInfoMessage> message = std::make_shared<FaceInfoMessage>(face);
    mManager->sendMessage(message);
}

// 检查人脸跟踪的状态
void FaceTrackManagerAgent::checkFaceTrackStatus()
{
    QMap<std::string, QMap<long long, std::shared_ptr<FaceTrackProcessor>>> allProcessor(mMapFaceTrackProcessor);
    QMapIterator<std::string, QMap<long long, std::shared_ptr<FaceTrackProcessor>>> iterAbstract(allProcessor);
    while(iterAbstract.hasNext())
    {
        iterAbstract.next();
        checkFaceTrackStatus(iterAbstract.value());

        // 本路流没有人脸跟踪任务，则从map中删除
        if (mMapFaceTrackProcessor.value(iterAbstract.key()).empty())
        {
            removeTrackProcessor(iterAbstract.key(), 0);
        }
    }
}

void FaceTrackManagerAgent::checkFaceTrackStatus(const  QMap<long long, std::shared_ptr<FaceTrackProcessor>> &abstractProcessors)
{
    QMapIterator<long long, std::shared_ptr<FaceTrackProcessor>> iterProcessor(abstractProcessors);
    while(iterProcessor.hasNext())
    {
        iterProcessor.next();
        std::shared_ptr<FaceTrackProcessor> processor = iterProcessor.value();
        if (processor->isTracking())
        {
            continue;
        }

        // 如果已经跟踪完毕，则取停止跟踪
        processor->uninit();
        removeTrackProcessor(processor->getTrackCondition()->getAbstractId(), processor->getFaceId());
    }
}

// 根据abstrace id,获取process
QMap<long long, std::shared_ptr<FaceTrackProcessor>> FaceTrackManagerAgent::getTrackProcessByAbstraceId(std::string abstraceId)
{
    QMap<long long, std::shared_ptr<FaceTrackProcessor>> result;
    return mMapFaceTrackProcessor.value(abstraceId, result);
}

// 获取人脸跟踪的处理类
std::shared_ptr<FaceTrackProcessor> FaceTrackManagerAgent::getTrackProcessor(std::string abstractId, long long faceId)
{
    std::shared_ptr<FaceTrackProcessor> result;
    if (mMapFaceTrackProcessor.count(abstractId) > 0 && mMapFaceTrackProcessor.value(abstractId).count(faceId) > 0)
    {
       result = mMapFaceTrackProcessor.value(abstractId).value(faceId);
    }
    return result;
}

// 删除人脸跟踪的处理类
bool FaceTrackManagerAgent::removeTrackProcessor(std::string abstractId, long long faceId)
{
    if (mMapFaceTrackProcessor.count(abstractId) < 0)
    {
        return false;
    }

    if (faceId <= 0)
    {
        // 如果face id小于等于0，则删除整个abstrace id的processor
        mMapFaceTrackProcessor.remove(abstractId);
    }
    else
    {
        // 删除对应的processor
        mMapFaceTrackProcessor[abstractId].remove(faceId);
    }
    LOG_I(mClassName, "remove face track process, abstract id:" << abstractId << ", face id:" << faceId);
    return true;
}

// 添加跟踪人脸的processor
bool FaceTrackManagerAgent::addTrackProcessor(std::shared_ptr<FaceTrackProcessor> processor)
{
    std::string abstractId = processor->getTrackCondition()->getAbstractId();
    if (mMapFaceTrackProcessor.count(abstractId) > 0)
    {
        mMapFaceTrackProcessor[abstractId][processor->getFaceId()] = processor;
    }
    else
    {
        QMap<long long, std::shared_ptr<FaceTrackProcessor>> mapTrackProcess;
        mapTrackProcess[processor->getFaceId()] = processor;
        mMapFaceTrackProcessor[abstractId] = mapTrackProcess;
    }
    LOG_I(mClassName, "add face track processor, abstract id:" << abstractId << ", face id:" << processor->getFaceId() << ", track condition:" << processor->getTrackCondition()->toString());
    return true;
}

// 删除所有的processor
void FaceTrackManagerAgent::removeAllTrackProcessor()
{
    QMap<std::string, QMap<long long, std::shared_ptr<FaceTrackProcessor>>> allProcessor(mMapFaceTrackProcessor);
    QMapIterator<std::string, QMap<long long, std::shared_ptr<FaceTrackProcessor>>> iter(allProcessor);
    while(iter.hasNext())
    {
        iter.next();
        stopTrack(iter.key());
    }
}

// 查找rect 在所有的人脸位置中的索引
int FaceTrackManagerAgent::rectInAllFacePositions(std::vector<QRect> &allFacePosition, QRect rect)
{
    int index = -1;
    int coincideArea = 0;
    for (int i = 0; i < allFacePosition.size(); ++i)
    {
        // 找出重合的区域
        QRect position = allFacePosition.at(i);
        QRect coincideRect = position.intersected(rect);
        if (!coincideRect.isValid())
        {
            continue;
        }

        // 找出最大的重合区域
        int coincide = (coincideRect.width() * coincideRect.height() * 100) / (rect.width() * rect.height());
        if (coincide > coincideArea)
        {
            coincideArea = coincide;
            index = i;
        }
    }

    // 重合的区域是否达到了阈值，如果没有达到，则认为没有找到
    if (mCoincideAreaThreshold < coincideArea)
    {
        index = -1;
    }

    return index;
}
