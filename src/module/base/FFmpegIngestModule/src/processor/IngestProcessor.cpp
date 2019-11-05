#include <QDateTime>
#include "FrameRatio.h"
#include "VideoFrameInfo.h"
#include "IngestProcessor.h"
#include "FFmpegIngestManagerAgent.h"
#include "opencv2/opencv.hpp"

IngestProcessor::IngestProcessor(FFmpegIngestManagerAgent *managerAgent, std::shared_ptr<IngestInfo> ingestInfo)
    :ThreadHandler("ffmpegIngest")
{
    mManagerAgent = managerAgent;
    mIngestInfo = ingestInfo;
}

// 打开流
std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> IngestProcessor::openStream()
{
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> result;

    // 如果打开成功，则开启取流的线程
    AVDictionary *option = NULL;
    av_dict_set(&option, "rtsp_transport", "tcp", 0);

    // 设置超时时间(单位是微秒)
    av_dict_set(&option, "stimeout", "2000000", 0);

    mFormatContext = avformat_alloc_context();

    // 打开流
    int ret = avformat_open_input(&mFormatContext, mIngestInfo->getStreamUrl().c_str(), NULL, &option);
    if (ret != 0)
    {
        LOG_E(mClassName, "open stream " << mIngestInfo->getStreamUrl() << " fail, ingest info:" << mIngestInfo->toString() << ", reason:" << getErrorReason(ret));
        result.second = Common::getError(getErrorReason(ret));
        return result;
    }

    // 获取流信息
    ret = avformat_find_stream_info(mFormatContext, NULL);
    if (ret < 0)
    {
        LOG_E(mClassName, "find stream info fail, code:" << ret << ", ingest info:" << mIngestInfo->toString() << ", reason:" << getErrorReason(ret));
        result.second = Common::getError(getErrorReason(ret));
        return result;
    }

    // 查找视频流信息
    for (int i = 0; i < mFormatContext->nb_streams; ++i)
    {
        AVStream * stream = mFormatContext->streams[i];
        if (NULL == stream)
        {
            continue;
        }

        if (AVMEDIA_TYPE_VIDEO == stream->codecpar->codec_type)
        {
            mVideoStream = stream;
            mVideoStreamIndex = i;
            break;
        }
    }

    // 如果没有找到视频信息，则返回
    if (NULL == mVideoStream)
    {
        LOG_E(mClassName, "not find video stream info, ingest info:" << mIngestInfo->toString());
        result.second = Common::getError("not find video stream info in FFmpeg format context");
        return result;
    }

    // 打开解码器
    mVideoCodec = avcodec_find_decoder(mVideoStream->codec->codec_id);
    if (NULL == mVideoCodec)
    {
        LOG_E(mClassName, "find codec fail, codec id:" << mVideoStream->codec->codec_id);
        result.second = Common::getError("not find video decoder");
        return result;
    }
    mVideoCodecContext = avcodec_alloc_context3(mVideoCodec);
    ret = avcodec_open2(mVideoCodecContext, mVideoCodec, NULL);
    if (ret < 0)
    {
        LOG_E(mClassName, "open codecer fail, reason:" << getErrorReason(ret));
        result.second = Common::getError("open codecer fail");
        return result;
    }

    // 初始化视频信息
    if (!initVideoFormat())
    {
        LOG_E(mClassName, "get video format fail");
        result.second = Common::getError("get video format fail");
        return result;
    }

    // 启动线程
    mIngestThread = std::make_shared<Thread>(this);
    mIngestThread->start();
    result.first = mVideoFormat;
    return result;
}

// 关闭流
bool IngestProcessor::closeStream()
{
    if (NULL == mIngestThread.get())
    {
        LOG_W(mClassName, "close ingest, but ingest thread is NULL, ingest info:" << mIngestInfo->toString());
        return true;
    }

    // 停止线程的运行
    mIngestThread->stop();

    // 关闭解码器
    if (NULL != mVideoCodecContext)
    {
        avcodec_close(mVideoCodecContext);
        avcodec_free_context(&mVideoCodecContext);
        mVideoCodecContext = NULL;
        mVideoCodec = NULL;
    }

    // 释放资源
    if (NULL != mFormatContext)
    {
        avformat_close_input(&mFormatContext);
        avformat_free_context(mFormatContext);
        mFormatContext = NULL;
    }
    if (NULL != mVideoStream)
    {
        mVideoStream = NULL;
    }

    LOG_I(mClassName, "stop ingest,ingest info:" << mIngestInfo->toString());
    return true;
}

// 获取拉流信息
std::shared_ptr<IngestInfo> IngestProcessor::getIngestInfo()
{
    return mIngestInfo;
}

// 获取视频格式
std::shared_ptr<VideoFormat> IngestProcessor::getVideoFormat()
{
    return mVideoFormat;
}

// 初始化视频格式信息
bool IngestProcessor::initVideoFormat()
{
    if (NULL == mVideoStream || NULL != mVideoFormat.get())
    {
        return false;
    }

    // 暂时只支持h264
    if (AV_CODEC_ID_H264 != mVideoStream->codecpar->codec_id && AV_CODEC_ID_HEVC != mVideoStream->codecpar->codec_id)
    {
        LOG_E(mClassName, "not support video format, codec id:" << mVideoStream->codecpar->codec_id);
        return false;
    }

    VideoFormatType type = (AV_CODEC_ID_H264 == mVideoStream->codecpar->codec_id) ? Video_Format_H264 : Video_Format_H265;
    int num = mVideoStream->avg_frame_rate.num;
    int den = mVideoStream->avg_frame_rate.den;

    // 如果获取不到帧率，强制设为25帧
    if (0 == num || 0 == den)
    {
        num = 25;
        den = 1;
    }
    std::shared_ptr<FrameRatio> frameRatio = std::make_shared<FrameRatio>(num, den);
    mVideoFormat = std::make_shared<VideoFormat>(mIngestInfo->getStreamId(),type , mVideoStream->codecpar->width, mVideoStream->codecpar->height, frameRatio);
    return true;
}

// 拉取视频流
std::shared_ptr<Error> IngestProcessor::workThread()
{
    std::shared_ptr<Error> error;
    LOG_I(mClassName, "begin thread work");
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    av_init_packet(packet);
    QDateTime startTime;
    int readFailCount = 0;
    SwsContext* swsContext = NULL;
    while (!isStop())
    {
        int ret = av_read_frame(mFormatContext, packet);

        // 记录开始时间
        if (!startTime.isValid())
        {
            startTime = QDateTime::currentDateTime();
        }

        // 读取失败
        if (ret < 0)
        {
            LOG_E(mClassName, "read freame fail, reason:" << getErrorReason(ret));
            if (AVERROR_EOF == ret)
            {
                mManagerAgent->ingestException(mIngestInfo, Stop_End_Of_File);
                break;
            }
            else
            {
                ++readFailCount;
                if (readFailCount > 25)
                {
                    // 如果一秒没有读取到数据，则认为出现了异常
                    mManagerAgent->ingestException(mIngestInfo, Stop_Ingest_Exception);
                    break;
                }
                else
                {
                    continue;
                }
            }
        }
        readFailCount = 0;

        // 丢弃非视频数据
        if (packet->stream_index != mVideoStreamIndex)
        {
            continue;
        }

        // 解码视频帧
        int gotPicturePtr = 0;
        ret = avcodec_decode_video2(mVideoCodecContext, frame, &gotPicturePtr, packet);
        if (ret < 0)
        {
            LOG_E(mClassName, "decoder video fail, ingest info:" << mIngestInfo->toString() << ", error reason:" << getErrorReason(ret));
            continue;
        }

        // 把YUV格式的视频转为Mat
        if (NULL == swsContext)
        {
            swsContext = sws_getContext(frame->width, frame->height, (AVPixelFormat) frame->format, frame->width, frame->height, AVPixelFormat::AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
        }
        std::shared_ptr<cv::Mat> imageMat = std::make_shared<cv::Mat>(frame->height, frame->width, CV_8UC3);
        int cvLinesizes[1] = {imageMat->step1()};
        sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, &imageMat->data, cvLinesizes);

        // 视频帧信息
        VideoFrameType frameType = (VideoFrameType)frame->pict_type;
        std::shared_ptr<std::string> frameData = std::make_shared<std::string>((char *)frame->data[0], frame->linesize[0]);
        std::shared_ptr<VideoFrameInfo> frameInfo = std::make_shared<VideoFrameInfo>(mIngestInfo->getStreamId(), mFrameIndex++, frameType , imageMat, packet->pts, packet->dts);

        // 发送视频帧
        mIngestInfo->addFrameCount();
        mManagerAgent->sendVideoFrame(frameInfo);

        // 判断视频播放是否结束
        if (mIngestInfo->getPlayDuration() > 0)
        {
             QDateTime current = QDateTime::currentDateTime();
             if ((current.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()) > mIngestInfo->getPlayDuration())
             {
                 stop();
                 mManagerAgent->ingestException(mIngestInfo, Stop_End_Of_File);
             }
        }
    }
    av_frame_free(&frame);
    frame = NULL;
    av_packet_free(&packet);
    packet = NULL;
    if (NULL != swsContext)
    {
        sws_freeContext(swsContext);
        swsContext = NULL;
    }
    LOG_I(mClassName, "end thread work");
    return error;
}

// 获取错误信息
std::string IngestProcessor::getErrorReason(int code)
{
    std::string result;
    char buf[1024] = {0};
    int ret = av_strerror(code, buf, sizeof(buf));
    if (0 == ret)
    {
        result = std::string(buf);
    }
    return result;
}
