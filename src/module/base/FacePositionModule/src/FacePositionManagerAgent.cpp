#include "IngestInfo.h"
#include "VideoFrameInfo.h"
#include "TrackCondition.h"
#include "FaceInfoMessage.h"
#include "VideoFrameMessage.h"
#include "FacePositionMessage.h"
#include "FacePositionManager.h"
#include "FaceTrackStopMessage.h"
#include "FacePositionProcessor.h"
#include "IngestExceptionMessage.h"
#include "FacePositionManagerAgent.h"

FacePositionManagerAgent::FacePositionManagerAgent(FacePositionManager *manager)
{
    mManager = manager;
}

// 初始化
bool FacePositionManagerAgent::init()
{
    return true;
}

// 反初始化
bool FacePositionManagerAgent::uninit()
{
    removeAllProcessor();
    return true;
}

// 开始跟踪
bool FacePositionManagerAgent::startTrack(std::shared_ptr<TrackCondition> trackInfo)
{
    // 判断是否已经存在
    std::shared_ptr<FacePositionProcessor> processor = getFacePositionPressor(trackInfo->getAbstractId());
    if (NULL != processor.get())
    {
        LOG_W(mClassName, "tracker already exist, ignore, tracker condition:" << trackInfo->toString());
        return true;
    }

    // 添加processor
    processor = addFacePositionPressor(trackInfo);

    // 处理缓冲区中的数据
    std::shared_ptr<Queue<VideoFrameMessage>> bufferQueue = mVideoFrameBuffer.value(trackInfo->getAbstractId());
    mVideoFrameBuffer.remove(trackInfo->getAbstractId());
    if (NULL == bufferQueue.get())
    {
        return true;
    }

    while(!bufferQueue->empty())
    {
        std::shared_ptr<VideoFrameMessage> message = bufferQueue->pop();
        if (NULL == message.get())
        {
            continue;
        }
        receiveVideoFrame(message);
    }
    return true;
}

// 清空跟踪的缓存
void FacePositionManagerAgent::clearTrackBuffer(std::shared_ptr<TrackCondition> trackInfo)
{
    // 清空的缓存
    mVideoFrameBuffer.remove(trackInfo->getAbstractId());
}

// 停止跟踪
bool FacePositionManagerAgent::stopTrack(std::string abstractId)
{
    // 判断是否已经存在
    std::shared_ptr<FacePositionProcessor> processor = getFacePositionPressor(abstractId);
    if (NULL == processor.get())
    {
        LOG_W(mClassName, "stop processor fail, not find track, id:" << abstractId);
        return false;
    }

    // 反初始化tracker
    processor->uninit();

    // 删除processor
    removeFacePositionProcessor(abstractId);
    return true;
}

// 拉流异常
bool FacePositionManagerAgent::IngestException(std::shared_ptr<IngestExceptionMessage> message)
{
    std::string abstractId = message->getIngestInfo()->getStreamId();
    std::shared_ptr<FacePositionProcessor> processor = getFacePositionPressor(abstractId);
    if (NULL != processor.get())
    {
        processor->receiveIngestException(message);
    }
    return true;
}

// 接收到视频帧
bool FacePositionManagerAgent::receiveVideoFrame(std::shared_ptr<VideoFrameMessage> message)
{
    std::string abstractId = message->getVideoFrameInfo()->getStreamId();
    std::shared_ptr<FacePositionProcessor> processor = getFacePositionPressor(abstractId);
    if (NULL == processor.get())
    {
        // 如果processor还没有建立，则先把数据缓存起来
        if (mVideoFrameBuffer.contains(abstractId))
        {
            mVideoFrameBuffer[abstractId]->push(message);
        }
        else
        {
            std::shared_ptr<Queue<VideoFrameMessage>> queue = std::make_shared<Queue<VideoFrameMessage>>();
            queue->push(message);
            mVideoFrameBuffer[abstractId] = queue;
        }
    }
    else
    {
        processor->receiveVideoFrame(message);
    }
    return true;
}

// 发送停止跟踪的消息
void FacePositionManagerAgent::sendStopTrackMessage(std::shared_ptr<TrackCondition> trackCondition)
{
    std::shared_ptr<FaceTrackStopMessage> message = std::make_shared<FaceTrackStopMessage>(trackCondition);
    mManager->sendMessage(message);
}

// 发送人脸位置信息
void FacePositionManagerAgent::sendFacePositionMessage(std::shared_ptr<TrackCondition> trackCondition, std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame)
{
    std::shared_ptr<FacePositionMessage> message = std::make_shared<FacePositionMessage>(trackCondition, videoFrame, faces, isDetectFrame);
    mManager->sendMessage(message);
}

// 删除一路人脸跟踪的视频流
bool FacePositionManagerAgent::removeFacePositionProcessor(std::string id)
{
    LOG_I(mClassName, "remove processor, id:" << id);
    return mMapFacePositionProcessor.remove(id);
}

// 删除所有的人脸跟踪的视频流
void FacePositionManagerAgent::removeAllProcessor()
{
    QMap<std::string, std::shared_ptr<FacePositionProcessor>> allProcessor(mMapFacePositionProcessor);
    QMapIterator<std::string, std::shared_ptr<FacePositionProcessor>> iter(allProcessor);
    while (iter.hasNext())
    {
        iter.next();
        stopTrack(iter.key());
    }
}

// 添加一路人脸跟踪的流
std::shared_ptr<FacePositionProcessor> FacePositionManagerAgent::addFacePositionPressor(std::shared_ptr<TrackCondition> trackInfo)
{
    if (mMapFacePositionProcessor.contains(trackInfo->getAbstractId()))
    {
        LOG_W(mClassName, "face track already exist, will override it, net track info:" << trackInfo->toString());
    }
    std::shared_ptr<FacePositionProcessor> processor = std::make_shared<FacePositionProcessor>(this);
    processor->init(trackInfo);
    mMapFacePositionProcessor[trackInfo->getAbstractId()] = processor;
    LOG_I(mClassName, "add processor, tracker condition:" << trackInfo->toString());
    return processor;
}

// 获取人脸跟踪处理类
std::shared_ptr<FacePositionProcessor> FacePositionManagerAgent::getFacePositionPressor(std::string id)
{
    std::shared_ptr<FacePositionProcessor> result;
    return mMapFacePositionProcessor.value(id, result);
}
