#include <QRect>
#include <vector>
#include <QDateTime>
#include "FaceTrack.h"
#include "FaceChoose.h"
#include "VideoFrameInfo.h"
#include "TrackCondition.h"
#include "VideoFrameMessage.h"
#include "FaceTrackManagerAgent.h"
#include "facedetection/facedetectcnn.h"
#include "opencv2/opencv.hpp"

#define DETECT_BUFFER_SIZE 0x20000

FaceTrack::FaceTrack(FaceTrackManagerAgent *managerAgent)
    :ThreadHandler("trackFace")
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
    while(!isStop() && !mMessageQueue.empty())
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
                chooseAllFace(true);
            }
            else if (Video_Frame_Message == message->getMessageType())
            {
                // 跟踪人脸
                std::shared_ptr<VideoFrameMessage> frame = std::dynamic_pointer_cast<VideoFrameMessage>(message);
                processVideoFrame(frame->getVideoFrameInfo());
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
    if ((now.toMSecsSinceEpoch() - faceChoose->getLastReceiveTime().toMSecsSinceEpoch()) > mFaceExpire)
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
            removeFace(faceId);
        }
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
    if (0 == (videoFrame->getFrameIndex() % mFaceDetectInterval))
    {
        // 人脸检测
        detectFacePosition(videoFrame);
    }
    else
    {
        // 人脸跟踪
        trackFacePosition(videoFrame);
    }
}

// 检测人脸
void FaceTrack::detectFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    // 初始化内存
    memset(mFaceDetectBuffer, 0, DETECT_BUFFER_SIZE);
    std::shared_ptr<cv::Mat> frameMat = Common::getMat(*videoFrame->getFrameData());
    int * detectResult = facedetect_cnn(mFaceDetectBuffer, (unsigned char*)(frameMat->ptr(0)), frameMat->cols, frameMat->rows, (int)frameMat->step);

    // 校对人脸
    std::vector<QRect> newFaceRects;
    for(int i = 0; i < (detectResult ? *detectResult : 0); i++)
    {
        short * p = ((short*)(detectResult+1))+142*i;
        int left = p[0];
        int top = p[1];
        int width = p[2];
        int height = p[3];
        //int confidence = p[4];
        int angle = p[5];

       QRect rect(left, top, width, height);
       newFaceRects.push_back(rect);
    }

    // 选择人脸
}

// 跟踪人脸位置
void FaceTrack::trackFacePosition(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    // 跟踪人脸

    // 选择人脸
}

// 发送跟踪到的人脸
void FaceTrack::sendTrackFaceResult(std::shared_ptr<TrackFaceInfo> face)
{

}
