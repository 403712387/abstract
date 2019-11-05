#include <QRect>
#include <vector>
#include <QDateTime>
#include "FaceTrack.h"
#include "FaceChoose.h"
#include "TrackCommon.h"
#include "TrackFaceInfo.h"
#include "VideoFrameInfo.h"
#include "TrackCondition.h"
#include "VideoFrameMessage.h"
#include "FaceTrackManagerAgent.h"
#include "facedetection/facedetectcnn.h"

#define DETECT_BUFFER_SIZE 0x20000

FaceTrack::FaceTrack(FaceTrackManagerAgent *managerAgent)
    :ThreadHandler("FaceTrack")
{
    mManagerAgent = managerAgent;
}

// 初始化
bool FaceTrack::init(std::shared_ptr<TrackCondition> trackInfo)
{
    LOG_I(mClassName, "begin face track, track info:" << trackInfo->toString());
    mTrackInfo = trackInfo;
    mFaceDetectBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
    mFaceDetectInterval = mTrackInfo->getFrameRate() * mFaceDetectIntervalRatio;
    mTrackThread = std::make_shared<Thread>(this);
    mTrackThread->start();
    return true;
}

// 反初始化
bool FaceTrack::uninit()
{
    LOG_I(mClassName, "end face track, track info:" << mTrackInfo->toString());
    mTrackThread->stop();

    // 释放人脸检测结果的内存
    if (NULL != mFaceDetectBuffer)
    {
        free(mFaceDetectBuffer);
        mFaceDetectBuffer = NULL;
    }
    return true;
}

// 接收到视频帧
void FaceTrack::receiveVideoFrame(std::shared_ptr<BaseMessage> message)
{
    mMessageQueue.push(message);
}

// 接收到拉流异常的消息
void FaceTrack::ingestException(std::shared_ptr<BaseMessage> message)
{
    mMessageQueue.push(message);
}

// 工作的线程
std::shared_ptr<Error> FaceTrack::workThread()
{
    LOG_I(mClassName, "begin face track thread, track info:" << mTrackInfo->toString());
    while(!isStop() || !mMessageQueue.empty())
    {
        std::shared_ptr<BaseMessage> message = mMessageQueue.popExpiration(10);
        if (NULL == message.get())
        {
            // 选择跟踪出来的质量较好的人脸
            chooseAllFace(false);
        }
        else
        {
            if (Ingest_Exception_Message == message->getMessageType())
            {
                // 拉流出现了异常，选择跟踪出来的质量较好的人脸
                LOG_I(mClassName, "ingest exception, choose all face, condition:" << mTrackInfo->toString());
                chooseAllFace(true);
            }
            else if (Video_Frame_Message == message->getMessageType())
            {
                // 跟踪人脸
                std::shared_ptr<VideoFrameMessage> frame = std::dynamic_pointer_cast<VideoFrameMessage>(message);
                processVideoFrame(frame->getVideoFrameInfo());
            }
            else
            {
                LOG_E(mClassName, "exception message, message info:" << message->toString());
            }
        }
    }

    // 停止跟踪，从队列中选择出来人脸质量较好的人脸
    chooseAllFace(true);
    LOG_I(mClassName, "end face track thread, track info:" << mTrackInfo->toString());
    return Common::noError();
}

// 选择人脸
void FaceTrack::chooseAllFace(bool trackDone)
{
    QMap<long long, std::shared_ptr<FaceChoose>> mapFaceChoose(mMapFaceChoose);
    QMapIterator<long long, std::shared_ptr<FaceChoose>> iterator(mapFaceChoose);
    while (iterator.hasNext())
    {
        iterator.next();
        chooseFaceFromList(iterator.key(), trackDone);
    }
}

// 选择人脸
bool FaceTrack::chooseFaceFromList(long long faceId, bool trackDone)
{
    std::shared_ptr<FaceChoose> faceChoose = mMapFaceChoose.value(faceId);
    if ( NULL == faceChoose.get())
    {
        return false;
    }

    // 判断是否超时
    QDateTime now = QDateTime::currentDateTime();
    if ((now.toMSecsSinceEpoch() - faceChoose->getLastReceiveTime().toMSecsSinceEpoch()) > mFaceExpire)  // 数据有积压的时候，这个地方会有异常
    {
        trackDone = true;
    }

    if (trackDone)      // 此人脸跟踪结束，取出所有的人脸
    {
        // 人脸跟踪结束，取出所有的人脸
        while(!faceChoose->chooseDone())
        {
            std::shared_ptr<TrackFaceInfo> face = faceChoose->getChooseFace();
            if (NULL == face.get())
            {
                break;
            }

            sendTrackFaceResult(face);
        }
        removeFace(faceId);
    }
    else
    {
        // 此人脸跟踪尚未结束
        int receiveFaceTrackCount = faceChoose->getReceiveFaceTrackCount();
        int chooseFaceCount = faceChoose->getReceiveFaceTrackCount();
        int videoFrameRatio = faceChoose->getTarckCondition()->getFrameRate();

        //  只有在实时优先的模式下，才会在跟踪完之前取人脸信息
        if (Abstract_Realtime_First != faceChoose->getTarckCondition()->getAbstractModel())
        {
            return true;
        }

        // 判断是否达到了取人脸图片的要求
        if (0 == chooseFaceCount && receiveFaceTrackCount > videoFrameRatio / 2)
        {
            std::shared_ptr<TrackFaceInfo> face = faceChoose->getChooseFace();
            if (NULL != face.get())
            {
                sendTrackFaceResult(face);
            }
        }
    }
    if (trackDone)
    {
        LOG_I(mClassName, "trace face done, face id:" << faceId << ", trace condition:" << mTrackInfo->toString());
    }

    return true;
}

// 删除人脸
bool FaceTrack::removeFace(long long faceId)
{
    return mMapFaceChoose.remove(faceId);
}

// 处理接收到的视频帧
void FaceTrack::processVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    // 人脸跟踪
    if (videoFrame->getFrameIndex() > 0)
    {
        trackFacePosition(videoFrame);
    }

    if (0 == (videoFrame->getFrameIndex() % mFaceDetectInterval))
    {
        // 人脸检测
        detectFacePosition(videoFrame);
    }
}

// 检测人脸
void FaceTrack::detectFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    // 初始化内存
    memset(mFaceDetectBuffer, 0, DETECT_BUFFER_SIZE);
    std::shared_ptr<cv::Mat> frameMat = videoFrame->getFrameMat();
    if (NULL == frameMat.get())
    {
        LOG_E(mClassName, "receive video frame , but Mat is NULL, track condition:" << mTrackInfo->toString() << ", video frame info:" << videoFrame->toString());
        return;
    }

    // 缩放人脸（减小图片分辨率,加快检测速度）
    static QRect maxRect(0, 0, 640, 360);
    std::shared_ptr<cv::Mat> image = frameMat;
    if (image->cols > maxRect.width() || image->rows > maxRect.height())
    {
        QRect imageRect(0, 0, image->cols, image->rows);
        if (1 == mImageCompressRatio)
        {
            // 图片的压缩率只用计算一次，因为视频的宽和高不会改变
            mImageCompressRatio = Common::getCompressRatio(imageRect, maxRect);
        }
        QRect compressRect(0, 0, imageRect.width() / mImageCompressRatio, imageRect.height() / mImageCompressRatio);
        image = Common::resizeMat(image.get(), compressRect);
    }

    // 检测人脸
    int *detectResult = facedetect_cnn(mFaceDetectBuffer, (unsigned char*)(image->ptr(0)), image->cols, image->rows, (int)image->step);

    // 检测出来的人脸的位置
    std::vector<cv::Rect> newFaceRects;
    for(int i = 0; i < (detectResult ? *detectResult : 0); i++)
    {
        short * temp = ((short*)(detectResult + 1)) + 142 * i;
        int left = temp[0] * mImageCompressRatio;
        int top = temp[1] * mImageCompressRatio;
        int width = temp[2] * mImageCompressRatio;
        int height = temp[3] * mImageCompressRatio;
        int confidence = temp[4];
        //int angle = temp[5];

        // 过滤掉比较小的人脸或者低置信度的人脸
        if (width < mMinFaceWidth || confidence < mMinConfidence)
        {
            LOG_D(mClassName, "igonre detect face, width:" << width << ", confidence:" << confidence);
            continue;
        }

        // 扩大roi区域
        QRect roiRect(left, top, width, height);
        roiRect = Common::extenedRoiRect(roiRect, 2, QRect(0, 0, frameMat->cols, frameMat->rows));
        newFaceRects.push_back(cv::Rect(0, 0, roiRect.width(), roiRect.height()));
    }

    // 更新了人脸位置
    updateFaceRect(videoFrame, image, newFaceRects);
}

// 跟踪人脸位置
void FaceTrack::trackFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    std::shared_ptr<cv::Mat> frameMat = videoFrame->getFrameMat();
    if (NULL == frameMat.get())
    {
        LOG_E(mClassName, "receive video frame , but Mat is NULL, track condition:" << mTrackInfo->toString() << ", video frame info:" << videoFrame->toString());
        return;
    }

    // 跟踪人脸
    std::set<long long> traceFaceIds;
    QMapIterator<long long, std::shared_ptr<FaceChoose>> iter(mMapFaceChoose);
    while(iter.hasNext())
    {
        iter.next();

        std::shared_ptr<FaceChoose> faceChoose = iter.value();
        bool ret = faceChoose->trackFace(frameMat, videoFrame->getBirthday());
        if (!ret)
        {
            traceFaceIds.insert(iter.key());
        }
    }

    // 处理跟踪结束的人脸
    for (long long faceId : traceFaceIds)
    {
        chooseFaceFromList(faceId, true);
    }
}

// 更新人脸位置
void FaceTrack::updateFaceRect(std::shared_ptr<VideoFrameInfo> videoFrame, std::shared_ptr<cv::Mat> imageMat, std::vector<cv::Rect> &faceNewRects)
{
    int coincideArea = 60;
    for (cv::Rect newPosition : faceNewRects)
    {
        std::pair<long long, int> ret = chooseCoincideFaceId(newPosition);
        if (ret.second < coincideArea)
        {
            // 创建新的人脸信息
            createFaceChoose(imageMat, newPosition, videoFrame->getBirthday());
            continue;
        }

        std::shared_ptr<FaceChoose> faceChoose = mMapFaceChoose.value(ret.first);
        if (NULL == faceChoose.get())
        {
            continue;
        }

        faceChoose->updateFacePosition(newPosition);
    }
}

// 选择一个重叠的面积最多的人脸
std::pair<long long, int> FaceTrack::chooseCoincideFaceId(cv::Rect newFacePosition)
{
    std::pair<long long, int> result = {-1, 0};
    QRect detectPosition(newFacePosition.x, newFacePosition.y, newFacePosition.width, newFacePosition.height);
    QMapIterator<long long, std::shared_ptr<FaceChoose>> iter(mMapFaceChoose);
    while(iter.hasNext())
    {
        iter.next();
        std::shared_ptr<FaceChoose> faceChoose = iter.value();
        QRect trackPosition = faceChoose->getLastReceiveFaceInfo()->getFaceRect();
        QRect coincidePosition = trackPosition.intersected(detectPosition);

        // 跟踪出来的和检测出来的矩形没有交集
        if (!coincidePosition.isValid())
        {
            continue;
        }

        // 计算交集的面积
        int coincide = (coincidePosition.width() * coincidePosition.height() * 100) / trackPosition.width() * trackPosition.height();
        if (coincide > result.second)
        {
            result.first = iter.key();
            result.second = coincide;
        }
    }
    return result;
}

// 创建新的人脸信息
bool FaceTrack::createFaceChoose(std::shared_ptr<cv::Mat> imageMat, cv::Rect facePosition, QDateTime imageDataBirthday)
{
    std::shared_ptr<FaceChoose> faceChoose = std::make_shared<FaceChoose>(getFaceId(), mTrackInfo, mTrackAlgorithm);
    faceChoose->init(imageMat, facePosition, imageDataBirthday);
    mMapFaceChoose[faceChoose->getFaceId()] = faceChoose;
    LOG_I(mClassName, "new face in video, face id:" << faceChoose->getFaceId() << ", position:" << Common::getRectString(QRect(facePosition.x, facePosition.y, facePosition.width, facePosition.height)) << ", trace condition:" << mTrackInfo->toString());
    return true;
}

// 发送跟踪到的人脸
void FaceTrack::sendTrackFaceResult(std::shared_ptr<TrackFaceInfo> face)
{
    LOG_D(mClassName, "send track face, face info:" << face->toString());
    mManagerAgent->sendTrackFace(face);
}
