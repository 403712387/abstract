#include "IngestInfo.h"
#include "FaceTrack.h"
#include "TrackCondition.h"
#include "FaceInfoMessage.h"
#include "VideoFrameInfo.h"
#include "FaceTrackManager.h"
#include "FaceTrackProcessor.h"
#include "FacePositionMessage.h"
#include "FaceTrackManagerAgent.h"
#include "IngestExceptionMessage.h"

FaceTrackManagerAgent::FaceTrackManagerAgent(FaceTrackManager *manager)
{
    mManager = manager;
}

// 初始化
bool FaceTrackManagerAgent::init()
{
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
        QMap<long long, std::shared_ptr<FaceTrackProcessor>> abstractProcessor =  iterAbstract.value();
        QMapIterator<long long, std::shared_ptr<FaceTrackProcessor>> iterProcessor(abstractProcessor);
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
            removeTrackProcessor(iterAbstract.key(), iterProcessor.key());
        }

        // 本路流没有人脸跟踪任务，则从map中删除
        if (mMapFaceTrackProcessor.value(iterAbstract.key()).empty())
        {
            removeTrackProcessor(iterAbstract.key(), 0);
        }
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
        removeTrackProcessor(iter.key(), 0);
    }
}

