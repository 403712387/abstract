#ifndef FACE_POSITION_PROCESSOR_H
#define FACE_POSITION_PROCESSOR_H
#include <string>
#include <memory>
#include <QRect>
#include <vector>
#include "Queue.h"
#include "Thread.h"
#include "ThreadHandler.h"

/*
 * 进行人脸的位置检测
 */
class BaseMessage;
class VideoFrameInfo;
class TrackCondition;
class FacePositionManagerAgent;
class FacePositionProcessor : public ThreadHandler
{
public:
    FacePositionProcessor(FacePositionManagerAgent *agent);

    // 初始化
    bool init(std::shared_ptr<TrackCondition> trackInfo);

    // 反初始化
    void uninit();

    // 接收到视频帧
    void receiveVideoFrame(std::shared_ptr<BaseMessage> message);

    // 接收到拉流异常的消息
    void receiveIngestException(std::shared_ptr<BaseMessage> message);

protected:
    // 人脸位置检测的线程
    virtual std::shared_ptr<Error> workThread();

private:
    // 处理视频帧
    bool processVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame);

    // 进行人脸位置检测
    std::vector<QRect> getFacePositionFromVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame);

    // 跟踪结束
    void sendStopTrackMessage();

    // 发送人脸位置消息
    void sendFacePositionMessage(std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame);

    // 改变人脸检测的帧间隔
    void changeFaceDetectInterval(int faceCount);

private:
    std::string                     mClassName = "FacePositionProcessor";
    FacePositionManagerAgent        *mManagerAgent= NULL;
    std::shared_ptr<TrackCondition> mTrackInfo;

    std::shared_ptr<Thread>         mPositionThread;       // 人脸检测线程
    Queue<BaseMessage>              mMessageQueue;

    float                           mFaceDetectIntervalRatio = 1.0;
    int                             mFaceDetectInterval = 24;       // 隔多少帧进行一次人脸检测

    unsigned char                   *mFaceDetectBuffer = NULL;       // 人脸检测的结果
    int                             mMinFaceWidth = 24;     // 最小的人脸大小，如果框的区域小于这个人脸，则忽略
    int                             mMinConfidence = 50;   // 低于此置信度的则认为不是人脸
    int                             mImageCompressRatio = 1;    // 检测人脸位置的时候把图片压缩一下，加快检测速度
};

#endif
