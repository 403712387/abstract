#ifndef FACE_TRACK_MANAGER_AGENT_H
#define FACE_TRACK_MANAGER_AGENT_H
#include <QMap>
#include "Queue.h"
#include "Common.h"

class TrackFaceInfo;
class TrackCondition;
class VideoFrameInfo;
class FaceTrackManager;
class FacePositionMessage;
class FaceTrackProcessor;
class IngestExceptionMessage;
class FaceTrackManagerAgent
{
public:
    FaceTrackManagerAgent(FaceTrackManager *manager);

    // 初始化
    bool init();

    // 反初始化
    bool uninit();

    // 停止跟踪
    bool stopTrack(std::string abstractId);

    // 处理人脸位置消息
    bool processFacePosition(std::shared_ptr<FacePositionMessage> message);

    // 发送跟踪出来的人脸
    void sendTrackFace(std::shared_ptr<TrackFaceInfo> face);

    // 检查人脸跟踪的状态
    void checkFaceTrackStatus();

private:
    // 根据abstrace id,获取process
    QMap<long long, std::shared_ptr<FaceTrackProcessor>> getTrackProcessByAbstraceId(std::string abstraceId);

    // 获取人脸跟踪的处理类
    std::shared_ptr<FaceTrackProcessor> getTrackProcessor(std::string abstractId, long long faceId);

    // 删除人脸跟踪的处理类
    bool removeTrackProcessor(std::string abstractId, long long faceId);

    // 删除所有的processor
    void removeAllTrackProcessor();

private:
    std::string             mClassName = "FaceTrackManagerAgent";
    FaceTrackManager        *mManager = NULL;

    QMap<std::string, QMap<long long, std::shared_ptr<FaceTrackProcessor>>>  mMapFaceTrackProcessor;    // 跟踪的处理类,QMap<abstractID, QMap<FaceId, processor>>
};

#endif
