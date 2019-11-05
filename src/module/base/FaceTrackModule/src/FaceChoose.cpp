#include <algorithm>
#include "FaceChoose.h"
#include "TrackFaceInfo.h"
#include "TrackCondition.h"
#include "TrackCondition.h"
#include "samples_utility.hpp"

FaceChoose::FaceChoose(long long faceId, std::shared_ptr<TrackCondition> trackCondition, std::string trackALgorithm)
{
    mFaceId = faceId;
    mTrackCondition = trackCondition;
    mTrackAlgorithmName = trackALgorithm;
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

// 初始化人脸信息
bool FaceChoose::init(std::shared_ptr<cv::Mat> imageMat, cv::Rect facePosition, QDateTime imageDataBirthday)
{
    // 创建跟踪算分类
    mTrackAlgorithm = createTrackerByName(mTrackAlgorithmName);
    if (NULL == mTrackAlgorithm.get())
    {
        LOG_E(mClassName, "create track fail, track algorithm:" << mTrackAlgorithmName);
        return false;
    }
    cv::Rect2d roiRect(facePosition.x, facePosition.y, facePosition.width, facePosition.height);
    mTrackAlgorithm->init(*imageMat, roiRect);

    // 初始化人脸位置
    QRect faceRect(facePosition.x, facePosition.y, facePosition.width, facePosition.height);
    int faceQuality = getFaceQuality(imageMat, facePosition);
    std::shared_ptr<TrackFaceInfo> faceInfo = std::make_shared<TrackFaceInfo>(mTrackCondition->getAbstractId(), mFaceId, imageMat, faceRect, faceQuality, imageDataBirthday);
    receiveFaceTrackInfo(faceInfo);
    LOG_I(mClassName, "init successful, first face info:" << faceInfo->toString() << ", algorithm:" << mTrackAlgorithmName);
    return true;
}

// 跟踪人脸
bool FaceChoose::trackFace(std::shared_ptr<cv::Mat> imageMat, QDateTime imageDataBirthday)
{
    cv::Rect2d currentPosition(0, 0, 0, 0);
    bool ret = mTrackAlgorithm->update(*imageMat, currentPosition);
    if (ret)
    {
        // 获取图片质量
        cv::Rect faceRect(currentPosition.x, currentPosition.y, currentPosition.width, currentPosition.height);
        int faceQuality = getFaceQuality(imageMat, faceRect);
        std::shared_ptr<TrackFaceInfo> faceInfo = std::make_shared<TrackFaceInfo>(mTrackCondition->getAbstractId(), mFaceId, imageMat, QRect(faceRect.x, faceRect.y, faceRect.width, faceRect.height), faceQuality, imageDataBirthday);
        receiveFaceTrackInfo(faceInfo);
    }
    return ret;
}

// 更新人脸位置
void FaceChoose::updateFacePosition(cv::Rect facePosition)
{
    // 创建跟踪算分类
    cv::Ptr<cv::Tracker> tracker = createTrackerByName(mTrackAlgorithmName);
    if (NULL == tracker.get())
    {
        LOG_E(mClassName, "upate face position fail, create track fail, track algorithm:" << mTrackAlgorithmName);
        return;
    }

    mTrackAlgorithm.reset();
    mTrackAlgorithm = tracker;
    cv::Rect2d roiRect(facePosition.x, facePosition.y, facePosition.width, facePosition.height);
    mTrackAlgorithm->init(*mLastReceiveFaceInfo->getImageMat(), roiRect);
}

// 接收人脸信息
void FaceChoose::receiveFaceTrackInfo(std::shared_ptr<TrackFaceInfo> face)
{
    ++mReceiveFaceCount;
    mLastFaceTime = QDateTime::currentDateTime();
    mLastReceiveFaceInfo = face;
    if (mListFaceInfo.size() < mListFaceCount)
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

// 获取人脸图片质量
int FaceChoose::getFaceQuality(std::shared_ptr<cv::Mat> imageMat, cv::Rect roiRect)
{
    return roiRect.width * roiRect.height;
}
