#ifndef FACE_CHOOSE_H
#define FACE_CHOOSE_H
#include <string>
#include <memory>
#include <vector>
#include <QDateTime>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>

/*
 * 从跟踪出来的人脸中选择出人脸图片
*/
class TrackCondition;
class TrackFaceInfo;
class FaceChoose
{
public:
    FaceChoose(long long faceId, std::shared_ptr<TrackCondition> trackCondition, std::string trackALgorithm);

    // 初始化人脸信息
    bool init(std::shared_ptr<cv::Mat> imageMat, cv::Rect facePosition, QDateTime imageDataBirthday);

    // 跟踪人脸
    bool trackFace(std::shared_ptr<cv::Mat> imageMat, QDateTime imageDataBirthday);

    // 更新人脸位置
    void updateFacePosition(cv::Rect facePosition);

    // 获取人脸ID
    long long getFaceId();

    // 获取跟踪的条件
    std::shared_ptr<TrackCondition> getTarckCondition();

    // 获取接收的最后一张人脸图片的信息
    std::shared_ptr<TrackFaceInfo> getLastReceiveFaceInfo();

    // 获取接收到的人脸的个数
    int getReceiveFaceTrackCount();

    // 最近接收到人脸的时间
    QDateTime getLastReceiveTime();

    // 获取选中的人脸
    std::shared_ptr<TrackFaceInfo> getChooseFace();

    // 选中的人脸的个数
    int getChooseFaceCount();

    // 选人脸是否结束
    bool chooseDone();

private:
    // 接收人脸信息
    void receiveFaceTrackInfo(std::shared_ptr<TrackFaceInfo> face);

    // 获取人脸图片质量
    int getFaceQuality(std::shared_ptr<cv::Mat> imageMat, cv::Rect roiRect);

private:
    std::string         mClassName = "FaceChoose";
    std::string         mTrackAlgorithmName = "KCF";
    long long           mFaceId = 0;
    std::shared_ptr<TrackCondition> mTrackCondition;
    int                 mReceiveFaceCount = 0;
    int                 mChooseFaceCount = 0;   // 已经选中的人脸
    cv::Ptr<cv::Tracker>    mTrackAlgorithm;

    QDateTime           mLastFaceTime;          // 最后一次的送达时间
    std::shared_ptr<TrackFaceInfo>  mLastReceiveFaceInfo;   // 接收到的最后一张人脸图片的信息
    int                 mListFaceCount = 5;
    int                 mLowFaceQuailtyInList = 0;      // 队列中最低的人脸质量
    std::vector<std::shared_ptr<TrackFaceInfo>>   mListFaceInfo;  // 接收到的人脸信息
};

#endif
