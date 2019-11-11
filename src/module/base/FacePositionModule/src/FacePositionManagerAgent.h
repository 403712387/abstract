#ifndef FACE_POSITION_MANAGER_AGENT_H
#define FACE_POSITION_MANAGER_AGENT_H
#include <QMap>
#include "Queue.h"
#include "Common.h"

class TrackFaceInfo;
class TrackCondition;
class VideoFrameInfo;
class VideoFrameMessage;
class FacePositionManager;
class FacePositionProcessor;
class IngestExceptionMessage;
class FacePositionManagerAgent
{
public:
    FacePositionManagerAgent(FacePositionManager *manager);

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

    // 发送停止跟踪的消息
    void sendStopTrackMessage(std::shared_ptr<TrackCondition> trackCondition);

    // 发送人脸位置信息
    void sendFacePositionMessage(std::shared_ptr<TrackCondition> trackCondition, std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame);

private:
    // 删除一路人脸跟踪的视频流
    bool removeFacePositionProcessor(std::string id);

    // 删除所有的人脸跟踪的视频流
    void removeAllProcessor();

    // 添加一路人脸跟踪的流
    std::shared_ptr<FacePositionProcessor> addFacePositionPressor(std::shared_ptr<TrackCondition> trackInfo);

    // 获取人脸跟踪处理类
    std::shared_ptr<FacePositionProcessor> getFacePositionPressor(std::string id);

private:
    std::string     mClassName = "FacePositionManagerAgent";
    FacePositionManager    *mManager = NULL;

    QMap<std::string, std::shared_ptr<FacePositionProcessor>>   mMapFacePositionProcessor;
    QMap<std::string, std::shared_ptr<Queue<VideoFrameMessage>>>   mVideoFrameBuffer;      // 缓存的视频帧
};

#endif
