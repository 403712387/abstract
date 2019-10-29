#include "FaceTrackManager.h"
#include "FaceTrackManagerAgent.h"

FaceTrackManager::FaceTrackManager(TrackManager *manager)
{
    mManager = manager;
}

// 初始化
bool FaceTrackManager::init()
{
    return true;
}

// 反初始化
bool FaceTrackManager::uninit()
{
    return true;
}

// 开始跟踪
bool FaceTrackManager::startTrack(std::shared_ptr<TrackCondition> trackInfo)
{
    return true;
}

// 停止跟踪
bool FaceTrackManager::stopTrack(std::string abstractId)
{
    return true;
}

// 暂停跟踪
bool FaceTrackManager::pauseTrack(std::string abstractId)
{
    return true;
}

// 接收到视频帧
bool FaceTrackManager::receiveVideoFrame(std::shared_ptr<VideoFrameInfo> frame)
{
    return true;
}
