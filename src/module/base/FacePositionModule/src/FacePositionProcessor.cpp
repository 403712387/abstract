#include "BaseMessage.h"
#include "VideoFrameInfo.h"
#include "TrackCondition.h"
#include "VideoFrameMessage.h"
#include "FacePositionProcessor.h"
#include "FacePositionManagerAgent.h"
#include "opencv2/opencv.hpp"
#include "facedetection/facedetectcnn.h"

#define DETECT_BUFFER_SIZE 0x20000

FacePositionProcessor::FacePositionProcessor(FacePositionManagerAgent *agent)
    :ThreadHandler("FacePosition")
{
    mManagerAgent = agent;
}

// 初始化
bool FacePositionProcessor::init(std::shared_ptr<TrackCondition> trackInfo)
{
    LOG_I(mClassName, "begin face position processor, track info:" << trackInfo->toString());
    mTrackInfo = trackInfo;
    mFaceDetectBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
    mFaceDetectInterval = mTrackInfo->getFrameRate() * mFaceDetectIntervalRatio;
    mPositionThread = std::make_shared<Thread>(this);
    mPositionThread->start();
    return true;
}

// 反初始化
void FacePositionProcessor::uninit()
{
    LOG_I(mClassName, "end face position processor, track info:" << mTrackInfo->toString());
    mPositionThread->stop();

    // 释放人脸检测结果的内存
    if (NULL != mFaceDetectBuffer)
    {
        free(mFaceDetectBuffer);
        mFaceDetectBuffer = NULL;
    }
    return;
}

// 接收到视频帧
void FacePositionProcessor::receiveVideoFrame(std::shared_ptr<BaseMessage> message)
{
    mMessageQueue.push(message);
}

// 接收到拉流异常的消息
void FacePositionProcessor::receiveIngestException(std::shared_ptr<BaseMessage> message)
{
    mMessageQueue.push(message);
}

// 人脸位置检测的线程
std::shared_ptr<Error> FacePositionProcessor::workThread()
{
    LOG_I(mClassName, "begin face position processor thread, track info:" << mTrackInfo->toString());
    while(!isStop() || !mMessageQueue.empty())
    {
        std::shared_ptr<BaseMessage> message = mMessageQueue.popExpiration(10);
        if (NULL == message.get())
        {
            continue;
        }

        if (Ingest_Exception_Message == message->getMessageType())
        {
            // 拉流出现了异常,通知跟踪模块
            sendStopTrackMessage();
        }
        else if (Video_Frame_Message == message->getMessageType())
        {
            // 处理视频帧
            std::shared_ptr<VideoFrameMessage> frame = std::dynamic_pointer_cast<VideoFrameMessage>(message);
            processVideoFrame(frame->getVideoFrameInfo());
        }
        else
        {
            LOG_E(mClassName, "exception message, message info:" << message->toString());
        }
    }

    // 发送停止跟踪的消息
    sendStopTrackMessage();
    LOG_I(mClassName, "end face position processor thread, track info:" << mTrackInfo->toString());
    return Common::noError();
}

// 处理视频帧
bool FacePositionProcessor::processVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    // 判断是否时检测帧
    std::vector<QRect> faces;
    bool isDetectFrame = (0 == (videoFrame->getFrameIndex() % mFaceDetectInterval));
    if (isDetectFrame)
    {
        faces = getFacePositionFromVideoFrame(videoFrame);

        // 根据检测出来的人脸个数，修改检测帧的间隔（当人流密集的话，要频繁检测，防止漏人）
        changeFaceDetectInterval(faces.size());
    }

    // 发送消息
    sendFacePositionMessage(videoFrame, faces, isDetectFrame);

    return isDetectFrame;
}

// 进行人脸位置检测
std::vector<QRect> FacePositionProcessor::getFacePositionFromVideoFrame(std::shared_ptr<VideoFrameInfo> videoFrame)
{
    std::vector<QRect> result;

    // 初始化内存
    memset(mFaceDetectBuffer, 0, DETECT_BUFFER_SIZE);
    std::shared_ptr<cv::Mat> frameMat = videoFrame->getFrameMat();
    if (NULL == frameMat.get())
    {
        LOG_E(mClassName, "receive video frame , but Mat is NULL, track condition:" << mTrackInfo->toString() << ", video frame info:" << videoFrame->toString());
        return result;
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
    for(int i = 0; i < (detectResult ? *detectResult : 0); i++)
    {
        short * temp = ((short*)(detectResult + 1)) + 142 * i;
        int left = temp[0] * mImageCompressRatio;
        int top = temp[1] * mImageCompressRatio;
        int width = temp[2] * mImageCompressRatio;
        int height = temp[3] * mImageCompressRatio;
        int confidence = temp[4];
        int angle = temp[5];
        LOG_D(mClassName, "track condition:" << mTrackInfo->toString() << ", left:" << left << ", top:" << top << ", width:" << width << ", height:" << height << ", image compress ratio:" << mImageCompressRatio << ", angle:" << angle);

        // 过滤掉比较小的人脸或者低置信度的人脸
        if (width < mMinFaceWidth || confidence < mMinConfidence)
        {
            LOG_D(mClassName, "igonre detect face, width:" << width << ", confidence:" << confidence);
            continue;
        }

        // 扩大roi区域
        QRect roiRect(left, top, width, height);
        roiRect = Common::extenedRoiRect(roiRect, 2, QRect(0, 0, frameMat->cols, frameMat->rows));
        result.push_back(roiRect);
    }
    return result;
}

// 跟踪结束
void FacePositionProcessor::sendStopTrackMessage()
{
    mManagerAgent->sendStopTrackMessage(mTrackInfo);
}

// 发送人脸位置消息
void FacePositionProcessor::sendFacePositionMessage(std::shared_ptr<VideoFrameInfo> videoFrame, std::vector<QRect> faces, bool isDetectFrame)
{
    mManagerAgent->sendFacePositionMessage(mTrackInfo, videoFrame, faces, isDetectFrame);
}

// 改变人脸检测的帧间隔
void FacePositionProcessor::changeFaceDetectInterval(int faceCount)
{
    if (faceCount <= 3)
    {
        mFaceDetectInterval = mTrackInfo->getFrameRate() * mFaceDetectIntervalRatio;
    }
    else if (faceCount <= 6)
    {
        mFaceDetectInterval = mTrackInfo->getFrameRate() * mFaceDetectIntervalRatio * 0.5;
    }
    else if (faceCount <= 9)
    {
        mFaceDetectInterval = mTrackInfo->getFrameRate() * mFaceDetectIntervalRatio * 0.3;
    }
    else if (faceCount > 9)
    {
        mFaceDetectInterval = mTrackInfo->getFrameRate() * mFaceDetectIntervalRatio * 0.25;
    }
}
