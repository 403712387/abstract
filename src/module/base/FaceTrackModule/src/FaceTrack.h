#ifndef FACE_TRACK_H
#define FACE_TRACK_H
#include <QMap>
#include "Queue.h"
#include "Thread.h"
#include "BaseMessage.h"
#include "ThreadHandler.h"
/*
 * 处理人脸跟踪逻辑的类
*/
class FaceChoose;
class TrackFaceInfo;
class VideoFrameInfo;
class TrackCondition;
class FaceTrackManagerAgent;
class FaceTrack : public ThreadHandler
{
public:
    FaceTrack(FaceTrackManagerAgent *managerAgent);

    // 初始化
    bool init(std::shared_ptr<TrackCondition> trackInfo);

    // 反初始化
    bool uninit();

    // 接收到视频帧
    void receiveVideoFrame(std::shared_ptr<BaseMessage> message);

    // 接收到拉流异常的消息
    void ingestException(std::shared_ptr<BaseMessage> message);

protected:

    // 工作的线程
    virtual std::shared_ptr<Error> workThread();

    // 选择人脸
    void chooseAllFace(bool trackDone);

    // 选择人脸
    bool chooseFaceFromList(long long faceId, bool trackDone);

    // 处理接收到的视频帧
    void processVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame);

    // 检测人脸
    void detectFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame);

    // 跟踪人脸位置
    void trackFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame);

    // 发送跟踪到的人脸
    void sendTrackFaceResult(std::shared_ptr<TrackFaceInfo> face);

    // 删除人脸
    bool removeFace(long long faceId);

private:
    std::string                     mClassName = "FaceTrack";
    FaceTrackManagerAgent           *mManagerAgent= NULL;
    std::shared_ptr<TrackCondition> mTrackInfo;

    std::shared_ptr<Thread>         mTrackThread;       // 跟踪类
    Queue<BaseMessage>              mMessageQueue;

    int                             mFaceExpire = 3000;     //人脸超时的时间
    float                           mFaceDetectIntervalRatio = 1.0;
    int                             mFaceDetectInterval = 24;       // 隔多少帧进行一次人脸检测
    QMap<long long, std::shared_ptr<FaceChoose>>    mMapFaceChoose;     // 跟踪出来的人脸放入此map中

    unsigned char                   *mFaceDetectBuffer = NULL;       // 人脸检测的结果
};

#endif
