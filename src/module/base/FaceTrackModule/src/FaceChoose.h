#ifndef FACE_CHOOSE_H
#define FACE_CHOOSE_H
#include <string>
#include <memory>
#include <vector>
#include <QDateTime>

/*
 * 从跟踪出来的人脸中选择出人脸图片
*/
class TrackCondition;
class TrackFaceInfo;
class FaceChoose
{
public:
    FaceChoose(long long faceId, std::shared_ptr<TrackCondition> trackCondition);

    // 获取人脸ID
    long long getFaceId();

    // 获取跟踪的条件
    std::shared_ptr<TrackCondition> getTarckCondition();

    // 接收人脸信息
    void receiveFaceTrackInfo(std::shared_ptr<TrackFaceInfo> face);

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
    std::string         mClassName = "FaceChoose";
    long long           mFaceId = 0;
    std::shared_ptr<TrackCondition> mTrackCondition;
    int                 mReceiveFaceCount = 0;
    int                 mChooseFaceCount = 0;   // 已经选中的人脸

    QDateTime           mLastFaceTime;          // 最后一次的送达时间
    int                 mListFaceCount = 5;
    int                 mLowFaceQuailtyInList = 0;      // 队列中最低的人脸质量
    std::vector<std::shared_ptr<TrackFaceInfo>>   mListFaceInfo;  // 接收到的人脸信息
};

#endif