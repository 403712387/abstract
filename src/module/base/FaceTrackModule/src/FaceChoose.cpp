#include <algorithm>
#include "FaceChoose.h"
#include "TrackFaceInfo.h"
#include "TrackCondition.h"

FaceChoose::FaceChoose(long long faceId, std::shared_ptr<TrackCondition> trackCondition)
{
    mFaceId = faceId;
    mTrackCondition = trackCondition;
    mListFaceCount = trackCondition->getAbstractFaceCount() * 2 + 2;
}

// 获取人脸ID
long long FaceChoose::getFaceId()
{
    return mFaceId;
}

// 获取跟踪的条件
std::shared_ptr<TrackCondition> FaceChoose::getTarckCondition()
{
    return mTrackCondition;
}

// 接收人脸信息
void FaceChoose::receiveFaceTrackInfo(std::shared_ptr<TrackFaceInfo> face)
{
    ++mReceiveFaceCount;
    mLastFaceTime = QDateTime::currentDateTime();
    mLastReceiveFaceInfo = face;
    if (mListFaceInfo.size() < mListFaceInfo.size())
    {
        mListFaceInfo.push_back(face);
    }
    else if (face->getFaceQuality() < mLowFaceQuailtyInList)
    {
        // 人脸质量比队列中最低的人脸质量还要低，忽略
    }
    else
    {
        // 新来的人脸质量较高，把队列中最低的人脸更新一下
        auto minElement = std::max_element(mListFaceInfo.begin(), mListFaceInfo.end(), [](const std::shared_ptr<TrackFaceInfo> &first, const std::shared_ptr<TrackFaceInfo> &second) ->bool
        {
            return (first->getFaceQuality()  < second->getFaceQuality());
        });

        std::shared_ptr<TrackFaceInfo> lowQualityFace = *minElement;
        if (lowQualityFace->getFaceQuality() < face->getFaceQuality())
        {
            mLowFaceQuailtyInList = face->getFaceQuality();
            *minElement = face;
        }
    }
}

// 获取接收的最后一张人脸图片的信息
std::shared_ptr<TrackFaceInfo> FaceChoose::getLastReceiveFaceInfo()
{
    return mLastReceiveFaceInfo;
}

// 获取接收到的人脸的个数
int FaceChoose::getReceiveFaceTrackCount()
{
    return mReceiveFaceCount;
}

// 最近接收到人脸的时间
QDateTime FaceChoose::getLastReceiveTime()
{
    return mLastFaceTime;
}

// 获取选中的人脸
std::shared_ptr<TrackFaceInfo> FaceChoose::getChooseFace()
{
    std::shared_ptr<TrackFaceInfo> result;
    if (mListFaceInfo.empty())
    {
        return result;
    }

    auto maxElement = std::max_element(mListFaceInfo.begin(), mListFaceInfo.end(), [](const std::shared_ptr<TrackFaceInfo> &first, const std::shared_ptr<TrackFaceInfo> &second) ->bool
    {
        return (first->getFaceQuality()  < second->getFaceQuality());
    });

    ++mChooseFaceCount;
    result = *maxElement;
    mListFaceInfo.erase(maxElement);
    return result;
}

// 选中的人脸的个数
int FaceChoose::getChooseFaceCount()
{
    return mChooseFaceCount;
}

// 选人脸是否结束
bool FaceChoose::chooseDone()
{
    if (mListFaceInfo.empty() || mChooseFaceCount >= mTrackCondition->getAbstractFaceCount())
    {
        return true;
    }
    return false;
}
