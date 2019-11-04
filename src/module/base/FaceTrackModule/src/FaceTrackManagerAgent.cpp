#include "IngestInfo.h"
#include "FaceTrack.h"
#include "TrackCondition.h"
#include "FaceInfoMessage.h"
#include "VideoFrameInfo.h"
#include "FaceTrackManager.h"
#include "VideoFrameMessage.h"
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
    removeAllTracker();
    return true;
}

// 开始跟踪
bool FaceTrackManagerAgent::startTrack(std::shared_ptr<TrackCondition> trackInfo)
{
    // 判断是否已经存在
    std::shared_ptr<FaceTrack> tracker = getTracker(trackInfo->getAbstractId());
    if (NULL != tracker.get())
    {
        LOG_W(mClassName, "tracker already exist, ignore, tracker condition:" << trackInfo->toString());
        return true;
    }

    // 添加tracker
    tracker = addTracker(trackInfo);

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
void FaceTrackManagerAgent::clearTrackBuffer(std::shared_ptr<TrackCondition> trackInfo)
{
    // 清空的缓存
    mVideoFrameBuffer.remove(trackInfo->getAbstractId());
}

// 停止跟踪
bool FaceTrackManagerAgent::stopTrack(std::string abstractId)
{
    // 判断是否已经存在
    std::shared_ptr<FaceTrack> tracker = getTracker(abstractId);
    if (NULL == tracker.get())
    {
        LOG_W(mClassName, "stop track fail, not find track, id:" << abstractId);
        return false;
    }

    // 反初始化tracker
    tracker->uninit();

    // 删除tracker
    removeTracker(abstractId);
    return true;
}

// 拉流异常
bool FaceTrackManagerAgent::IngestException(std::shared_ptr<IngestExceptionMessage> message)
{
    std::string abstractId = message->getIngestInfo()->getStreamId();
    std::shared_ptr<FaceTrack> tracker = getTracker(abstractId);
    if (NULL != tracker.get())
    {
        tracker->ingestException(message);
    }
    return true;
}

// 接收到视频帧
bool FaceTrackManagerAgent::receiveVideoFrame(std::shared_ptr<VideoFrameMessage> message)
{
    std::string abstractId = message->getVideoFrameInfo()->getStreamId();
    std::shared_ptr<FaceTrack> tracker = getTracker(abstractId);
    if (NULL == tracker.get())
    {
        // 如果tracker还没有建立，则先把数据缓存起来
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
        tracker->receiveVideoFrame(message);
    }
    return true;
}

// 发送跟踪出来的人脸
void FaceTrackManagerAgent::sendTrackFace(std::shared_ptr<TrackFaceInfo> face)
{
    std::shared_ptr<FaceInfoMessage> message = std::make_shared<FaceInfoMessage>(face);
    mManager->sendMessage(message);
}

// 删除一路人脸跟踪的视频流
bool FaceTrackManagerAgent::removeTracker(std::string id)
{
    LOG_I(mClassName, "remove tracker, id:" << id);
    return mMapFaceTrack.remove(id);
}

// 删除所有的人脸跟踪的视频流
void FaceTrackManagerAgent::removeAllTracker()
{
    QMapIterator<std::string, std::shared_ptr<FaceTrack>> iter(mMapFaceTrack);
    while (iter.hasNext())
    {
        iter.next();
        removeTracker(iter.key());
    }
}

// 添加一路人脸跟踪的流
std::shared_ptr<FaceTrack> FaceTrackManagerAgent::addTracker(std::shared_ptr<TrackCondition> trackInfo)
{
    if (mMapFaceTrack.contains(trackInfo->getAbstractId()))
    {
        LOG_W(mClassName, "face track already exist, will override it, net track info:" << trackInfo->toString());
    }
    std::shared_ptr<FaceTrack> track = std::make_shared<FaceTrack>(this);
    track->init(trackInfo);
    mMapFaceTrack[trackInfo->getAbstractId()] = track;
    LOG_I(mClassName, "add tracker, tracker condition:" << trackInfo->toString());
    return track;
}

// 获取人脸跟踪处理类
std::shared_ptr<FaceTrack> FaceTrackManagerAgent::getTracker(std::string id)
{
    std::shared_ptr<FaceTrack> result;
    return mMapFaceTrack.value(id, result);
}
