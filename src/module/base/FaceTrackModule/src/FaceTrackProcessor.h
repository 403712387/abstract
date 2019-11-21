#ifndef FACE_TRACK_PROCESSOR_H
#define FACE_TRACK_PROCESSOR_H
#include <vector>
#include <string>
#include <QRect>
#include <QDateTime>
#include "Queue.h"
#include "Thread.h"
#include "ThreadHandler.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/tracking.hpp"
#include "opencv2/videoio.hpp"

class BaseMessage;
class TrackFaceInfo;
class TrackCondition;
class VideoFrameInfo;
class FaceTrackManagerAgent;
class FaceTrackProcessor : public ThreadHandler
{
public:
    FaceTrackProcessor(FaceTrackManagerAgent *agent);

    // 初始化
    bool init(std::shared_ptr<TrackCondition> trackCondition, std::shared_ptr<VideoFrameInfo> videoFrame, QRect face);

    // 反初始化
    void uninit();

    // 获取跟踪条件
    std::shared_ptr<TrackCondition> getTrackCondition();

    // 获取faceID
    long long getFaceId();

    // 是否正在跟踪人脸
    bool isTracking();

    // 接收到人脸位置消息
    void receiveFacePositionMessage(std::shared_ptr<BaseMessage> message);

    // 获取当前的人脸位置
    QRect getCurrentFacePosition();

protected:
    // 人脸位置检测的线程
    virtual std::shared_ptr<Error> workThread();

private:
    // 处理视频帧
    bool processVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame);

    // 检查高质量人脸队列
    void checkHighQualityFaceList();

    // 获取人脸图片质量
    int getFaceQuality(std::shared_ptr<cv::Mat> imageMat, QRect faceRect);

    // 人脸是否发送完毕
    bool isSendFaceDone();

    // 从高质量人脸队列中取出一张人脸图片
    std::shared_ptr<TrackFaceInfo> getFaceInfoFromHighQualityFaceList();

    // 发送跟踪出来的人脸图片信息
    void sendFaceInfo(std::shared_ptr<TrackFaceInfo> faceInfo);

    // 矫正人脸框的位置
    void correctFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces);

    // 更新高质量的人脸队列
    void updateHighQualityFaceList(std::shared_ptr<VideoFrameInfo> videoFrame);

private:
    std::string                     mClassName = "FaceTrackProcessor";
    FaceTrackManagerAgent           *mManagerAgent = NULL;
    std::string                     mTrackAlgorithmName = "KCF";    // 跟踪算法名称(支持的算法： KCF, BOOSTING, MOSSE, MEDIAN_FLOW, MIL, GOTURN, CSRT)
    cv::Ptr<cv::Tracker>            mTrackAlgorithm;                // 人脸跟踪类
    std::shared_ptr<TrackCondition> mTrackCondition;
    long long                       mFaceId = 0;    // 人脸的ID(每一个人脸都有一个唯一ID)
    bool                            mIsTracking = true; // 是否正在跟踪

    std::shared_ptr<Thread>         mTrackThread;       // 人脸跟踪线程
    Queue<BaseMessage>              mMessageQueue;      // 消息队列
    int                             mCoincideAreaThreshold = 60;        // 人脸重叠剪辑的阈值，如果跟踪和检测的人脸位置重合区域超过阈值，则认为是同一个人

    QRect                           mCurrentFacePosition;       // 当前的人脸位置
    int                             mHighQualityFaceListSize = 6;       // 高质量人脸队列的大小
    int                             mLowestFaceQualityInList = 0;       // 队列中最低的人脸质量
    std::vector<std::shared_ptr<TrackFaceInfo>>   mHighQualityFaceList;  // 选择出来的高质量的人脸

    int                             mReceiveVideoFrameCount = 0;    // 接收到的视频帧的数量
    int                             mSendFaceCount = 0;             // 已经发送出去的人脸的个数
};

#endif
