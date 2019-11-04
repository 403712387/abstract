#ifndef FACE_TRACK_MANAGER_AGENT_H
#define FACE_TRACK_MANAGER_AGENT_H
#include "Common.h"

class TrackFaceInfo;
class TrackCondition;
class VideoFrameInfo;
class FaceTrackManager;
class FaceTrackManagerAgent
{
public:
    FaceTrackManagerAgent(FaceTrackManager *manager);

    // 初始化
    bool init();

    // 反初始化
    bool uninit();

    // 开始跟踪
    bool startTrack(std::shared_ptr<TrackCondition> trackInfo);

    // 停止跟踪
    bool stopTrack(std::string abstractId);

    // 暂停跟踪
    bool pauseTrack(std::string abstractId);

    // 接收到视频帧
    bool receiveVideoFrame(std::shared_ptr<VideoFrameInfo> frame);

    // 发送跟踪出来的人脸
    void sendTrackFace(std::shared_ptr<TrackFaceInfo> face);

private:
    std::string     mClassName = "FaceTrackManagerAgent";
    FaceTrackManager    *mManager = NULL;
};

#endif
