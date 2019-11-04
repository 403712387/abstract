#include "FaceTrackManager.h"
#include "FaceTrackManagerAgent.h"

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
    return true;
}

// 开始跟踪
bool FaceTrackManagerAgent::startTrack(std::shared_ptr<TrackCondition> trackInfo)
{
    return true;
}

// 停止跟踪
bool FaceTrackManagerAgent::stopTrack(std::string abstractId)
{
    return true;
}

// 暂停跟踪
bool FaceTrackManagerAgent::pauseTrack(std::string abstractId)
{
    return true;
}

// 接收到视频帧
bool FaceTrackManagerAgent::receiveVideoFrame(std::shared_ptr<VideoFrameInfo> frame)
{
    return true;
}

// 发送跟踪出来的人脸
void FaceTrackManagerAgent::sendTrackFace(std::shared_ptr<TrackFaceInfo> face)
{

}
