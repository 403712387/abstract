#include "TrackCommon.h"
#include "TrackFaceInfo.h"
#include "TrackCondition.h"
#include "VideoFrameInfo.h"
#include "samples_utility.hpp"
#include "FaceTrackProcessor.h"
#include "FacePositionMessage.h"
#include "FaceTrackManagerAgent.h"

FaceTrackProcessor::FaceTrackProcessor(FaceTrackManagerAgent *agent)
    :ThreadHandler("FaceTrack")
{
    mManagerAgent = agent;
    mFaceId = TrackCommon::getFaceId();
}

// 初始化
bool FaceTrackProcessor::init(std::shared_ptr<TrackCondition> trackCondition, std::shared_ptr<VideoFrameInfo> videoFrame, QRect face)
{
    LOG_I(mClassName, "begin face track processor, track info:" << trackCondition->toString() << ", face id:" << mFaceId);

    // 创建跟踪算法
    mTrackCondition = trackCondition;
    mTrackAlgorithm = createTrackerByName(mTrackAlgorithmName);
    if (NULL == mTrackAlgorithm.get())
    {
        LOG_E(mClassName, "create track fail, track algorithm:" << mTrackAlgorithmName);
        return false;
    }

    // 初始化跟踪位置
    mTrackAlgorithm->init(*(videoFrame->getFrameMat()), cv::Rect2d(face.left(), face.top(), face.width(), face.height()));
    mCurrentFacePosition = face;
    ++mReceiveVideoFrameCount;
    updateHighQualityFaceList(videoFrame);

    // 启动跟踪线程
    mHighQualityFaceListSize = trackCondition->getAbstractFaceCount() * 2 + 2;
    mTrackThread = std::make_shared<Thread>(this);
    mTrackThread->start();
    return true;
}

// 反初始化
void FaceTrackProcessor::uninit()
{
    LOG_I(mClassName, "end face track processor, track info:" << mTrackCondition->toString() << ", face id:" << mFaceId);
    mTrackThread->stop();
}

// 获取跟踪条件
std::shared_ptr<TrackCondition> FaceTrackProcessor::getTrackCondition()
{
    return mTrackCondition;
}

// 获取faceID
long long FaceTrackProcessor::getFaceId()
{
    return mFaceId;
}

// 是否正在跟踪人脸
bool FaceTrackProcessor::isTracking()
{
    return mIsTracking;
}

// 接收到人脸位置消息
void FaceTrackProcessor::receiveFacePositionMessage(std::shared_ptr<BaseMessage> message)
{
    mMessageQueue.push(message);
}

// 获取当前的人脸位置
QRect FaceTrackProcessor::getCurrentFacePosition()
{
    return mCurrentFacePosition;
}

// 人脸位置检测的线程
std::shared_ptr<Error> FaceTrackProcessor::workThread()
{
    LOG_I(mClassName, "begin face track processor thread, track info:" << mTrackCondition->toString() << ", face id:" << mFaceId);
    while(!isStop() || !mMessageQueue.empty())
    {
        std::shared_ptr<BaseMessage> message = mMessageQueue.popExpiration(400);
        if (NULL == message.get())
        {
            // 检查高质量人脸队列
#if 1
            LOG_I(mClassName, "message is NULL, begin check high face list, face id:" << mFaceId);
#endif
            //checkHighQualityFaceList();
#if 1
            LOG_I(mClassName, "message is NULL, end check high face list, face id:" << mFaceId);
#endif
            continue;
        }

        if (Face_Position_Message == message->getMessageType())
        {
#if 1
            LOG_I(mClassName, "begin process video frame, face id:" << mFaceId);
#endif
            // 跟踪人脸
            std::shared_ptr<FacePositionMessage> positionMessage = std::dynamic_pointer_cast<FacePositionMessage>(message);
            mIsTracking = processVideoFrame(positionMessage->getVideoFrameInfo(), positionMessage->getFacePositions(), positionMessage->isDetectFrame());
#if 1
            LOG_I(mClassName, "end process video frame, face id:" << mFaceId);
#endif
            // 选择跟踪出来的人脸
#if 1
            LOG_I(mClassName, "begin check high face list, face id:" << mFaceId);
#endif
            checkHighQualityFaceList();
#if 1
            LOG_I(mClassName, "begin check high face list, face id:" << mFaceId);
#endif
        }
        else
        {
            LOG_E(mClassName, "exception message, message info:" << message->toString());
        }
    }

    // 选择跟踪出来的人脸
    checkHighQualityFaceList();
    LOG_I(mClassName, "end face position processor thread, track info:" << mTrackCondition->toString() << ", face id:" << mFaceId);
    return Common::noError();
}

// 处理人脸位置信息(注意，如果isDetectFrame为false,说明这一帧是非检测帧)
bool FaceTrackProcessor::processVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame)
{
    // 判断跟踪是否已经结束
    if (!mIsTracking)
    {
        return false;
    }

    ++mReceiveVideoFrameCount;

    // 跟踪人脸
    cv::Rect2d position(0, 0, 0, 0);
    bool ret = mTrackAlgorithm->update(*(videoFrame->getFrameMat()), position);
    mCurrentFacePosition = QRect(position.x, position.y, position.width, position.height);
    mCurrentFacePosition = mCurrentFacePosition.intersected(QRect(0, 0, videoFrame->getFrameMat()->cols, videoFrame->getFrameMat()->rows));
    if (!ret)
    {
        LOG_I(mClassName, "face track done, face id" << mFaceId << ", receive video frame count:" << mReceiveVideoFrameCount << ", heigh face quality list size:" << mHighQualityFaceList.size() << ", lowest face quality in list:" << mLowestFaceQualityInList);
    }

    // 更新高质量人脸队列
    updateHighQualityFaceList(videoFrame);

    // 矫正人脸位置
    if (isDetectFrame && faces.size() > 0)
    {
        correctFacePosition(videoFrame, faces);
    }
    return ret;
}

// 更新高质量的人脸队列
void FaceTrackProcessor::updateHighQualityFaceList(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    int faceQuality = getFaceQuality(videoFrame->getFrameMat(), mCurrentFacePosition);
    if (mHighQualityFaceList.size() < mHighQualityFaceListSize)
    {
        std::shared_ptr<TrackFaceInfo> trackFace = std::make_shared<TrackFaceInfo>(mTrackCondition->getAbstractId(), mFaceId, videoFrame->getFrameMat(), mCurrentFacePosition, faceQuality, videoFrame->getBirthday());
        mHighQualityFaceList.push_back(trackFace);
    }
    else if (faceQuality < mLowestFaceQualityInList)
    {
        // 人脸质量比队列中最低的人脸质量还要低，忽略
    }
    else
    {
        // 新来的人脸质量较高，把队列中最低的人脸更新一下
        std::shared_ptr<TrackFaceInfo> trackFace = std::make_shared<TrackFaceInfo>(mTrackCondition->getAbstractId(), mFaceId, videoFrame->getFrameMat(), mCurrentFacePosition, faceQuality, videoFrame->getBirthday());
        auto minElement = std::max_element(mHighQualityFaceList.begin(), mHighQualityFaceList.end(), [](const std::shared_ptr<TrackFaceInfo> &first, const std::shared_ptr<TrackFaceInfo> &second) ->bool
        {
            return (first->getFaceQuality()  < second->getFaceQuality());
        });

        std::shared_ptr<TrackFaceInfo> lowQualityFace = *minElement;
        if (lowQualityFace->getFaceQuality() < faceQuality)
        {
            *minElement = trackFace;
        }

        // 更新队列中最低的人脸质量
        minElement = std::max_element(mHighQualityFaceList.begin(), mHighQualityFaceList.end(), [](const std::shared_ptr<TrackFaceInfo> &first, const std::shared_ptr<TrackFaceInfo> &second) ->bool
        {
            return (first->getFaceQuality()  < second->getFaceQuality());
        });
        mLowestFaceQualityInList = (*minElement)->getFaceQuality();
    }
}

// 检查高质量人脸队列
void FaceTrackProcessor::checkHighQualityFaceList()
{
    if (mIsTracking)
    {
        // 实时优先模式下，尽早的选出一张人脸图片
        if (Abstract_Realtime_First == mTrackCondition->getAbstractModel())
        {
            // 判断是否达到了取人脸图片的要求
            if (0 == mSendFaceCount && mReceiveVideoFrameCount > mTrackCondition->getFrameRate() / 2)
            {
                std::shared_ptr<TrackFaceInfo> face = getFaceInfoFromHighQualityFaceList();
                if (NULL != face.get())
                {
                    sendFaceInfo(face);
                }
            }
        }
    }
    else
    {
        // 选出指定张数的人脸图片
        while(!isSendFaceDone())
        {
            std::shared_ptr<TrackFaceInfo> face = getFaceInfoFromHighQualityFaceList();
            if (NULL == face.get())
            {
                break;
            }

            // 发送人脸图片信息
            sendFaceInfo(face);
        }
    }
}

// 获取人脸图片质量
int FaceTrackProcessor::getFaceQuality(std::shared_ptr<cv::Mat> imageMat, QRect faceRect)
{
    return faceRect.width() * faceRect.height();
}

// 人脸是否发送完毕
bool FaceTrackProcessor::isSendFaceDone()
{
    // 发送的图片张数达到了预期
    if (mSendFaceCount >= mTrackCondition->getAbstractFaceCount())
    {
        return true;
    }

    // 发送的人脸图片张数没有达到预期，但是没有人脸图片了
    if (mReceiveVideoFrameCount > 0 && mHighQualityFaceList.empty())
    {
        return true;
    }
    return false;
}

// 从高质量人脸队列中取出一张人脸图片
std::shared_ptr<TrackFaceInfo> FaceTrackProcessor::getFaceInfoFromHighQualityFaceList()
{
    std::shared_ptr<TrackFaceInfo> result;
    if (mHighQualityFaceList.empty())
    {
        return result;
    }

    // 查找人脸质量最高的人脸
    auto selectFace = std::max_element(mHighQualityFaceList.begin(), mHighQualityFaceList.end(), [](const std::shared_ptr<TrackFaceInfo> &first, const std::shared_ptr<TrackFaceInfo> &second) ->bool
    {
        return (first->getFaceQuality()  < second->getFaceQuality());
    });

    result = *selectFace;
    mHighQualityFaceList.erase(selectFace);
    return result;
}

// 发送跟踪出来的人脸图片信息
void FaceTrackProcessor::sendFaceInfo(std::shared_ptr<TrackFaceInfo> faceInfo)
{
    ++mSendFaceCount;
    LOG_I(mClassName, "send face info, info:" << faceInfo->toString());
    mManagerAgent->sendTrackFace(faceInfo);
}

// 矫正人脸框的位置
void FaceTrackProcessor::correctFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces)
{
    // 找到人脸框的位置
    QRect correctPosition;
    int maxCoincideArea = 0;
    for (QRect &position : faces)
    {
        QRect coincideRect = position.intersected(mCurrentFacePosition);
        if (coincideRect.isValid())
        {
            int coincideArea = (coincideRect.width() * coincideRect.height() * 100) / (mCurrentFacePosition.width() * mCurrentFacePosition.height());
            if (coincideArea > maxCoincideArea)
            {
                maxCoincideArea = coincideArea;
                correctPosition = position;
            }
        }
    }

    // 创建跟踪算分类
    cv::Ptr<cv::Tracker> tracker = createTrackerByName(mTrackAlgorithmName);
    if (NULL == tracker.get())
    {
        LOG_E(mClassName, "upate face position fail, create track fail, track algorithm:" << mTrackAlgorithmName);
        return;
    }

    // 更新跟踪算法中人脸框的位置
    mTrackAlgorithm.reset();
    mTrackAlgorithm = tracker;
    cv::Rect2d roiRect(mCurrentFacePosition.left(), mCurrentFacePosition.top(), mCurrentFacePosition.width(), mCurrentFacePosition.height());
    mTrackAlgorithm->init(*videoFrame->getFrameMat(), roiRect);
}
