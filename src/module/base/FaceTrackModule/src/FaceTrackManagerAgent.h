#ifndef FACE_TRACK_MANAGER_AGENT_H
#define FACE_TRACK_MANAGER_AGENT_H
#include <QMap>
#include "Queue.h"
#include "Common.h"

class FaceTrack;
class TrackFaceInfo;
class TrackCondition;
class VideoFrameInfo;
class FaceTrackManager;
class VideoFrameMessage;
class IngestExceptionMessage;
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

    // 清空跟踪的缓存
    void clearTrackBuffer(std::shared_ptr<TrackCondition> trackInfo);

    // 停止跟踪
    bool stopTrack(std::string abstractId);

    // 拉流异常
    bool IngestException(std::shared_ptr<IngestExceptionMessage> message);

    // 接收到视频帧
    bool receiveVideoFrame(std::shared_ptr<VideoFrameMessage> message);

    // 发送跟踪出来的人脸
    void sendTrackFace(std::shared_ptr<TrackFaceInfo> face);

private:
    // 删除一路人脸跟踪的视频流
    bool removeTracker(std::string id);

    // 删除所有的人脸跟踪的视频流
    void removeAllTracker();

    // 添加一路人脸跟踪的流
    std::shared_ptr<FaceTrack> addTracker(std::shared_ptr<TrackCondition> trackInfo);

    // 获取人脸跟踪处理类
    std::shared_ptr<FaceTrack> getTracker(std::string id);

private:
    std::string     mClassName = "FaceTrackManagerAgent";
    FaceTrackManager    *mManager = NULL;

    QMap<std::string, std::shared_ptr<FaceTrack>>        mMapFaceTrack;
    QMap<std::string, std::shared_ptr<Queue<VideoFrameMessage>>>   mVideoFrameBuffer;      // 缓存的视频帧
};

#endif
