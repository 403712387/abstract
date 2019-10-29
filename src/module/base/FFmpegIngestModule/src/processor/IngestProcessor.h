#ifndef INGEST_PROCESSOR_H
#define INGEST_PROCESSOR_H
#include <memory>
#include "Thread.h"
#include "ThreadHandler.h"
#include "VideoFormat.h"
#include "IngestInfo.h"

extern "C"
{
#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libavdevice/avdevice.h"
#include "ffmpeg/libavformat/avformat.h"
#include "ffmpeg/libavfilter/avfilter.h"
#include "ffmpeg/libavutil/avutil.h"
#include "ffmpeg/libswscale/swscale.h"
#include "ffmpeg/libavutil/dict.h"
}

// 使用ffmpeg拉取视频流
class FFmpegIngestManagerAgent;
class IngestProcessor : public ThreadHandler
{
public:
    IngestProcessor(FFmpegIngestManagerAgent *managerAgent, std::shared_ptr<IngestInfo> ingestInfo);

    // 打开流
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> openStream();

    // 关闭流
    bool closeStream();

    // 获取拉流信息
    std::shared_ptr<IngestInfo> getIngestInfo();

    // 获取视频格式
    std::shared_ptr<VideoFormat> getVideoFormat();

protected:

    // 拉取视频流
    virtual std::shared_ptr<Error> workThread();

    // 初始化视频格式信息
    bool initVideoFormat();

    // 获取错误信息
    std::string getErrorReason(int code);

private:
    std::string         mClassName = "IngestProcessor";
    FFmpegIngestManagerAgent *mManagerAgent = NULL;
    std::shared_ptr<VideoFormat>        mVideoFormat;
    std::shared_ptr<IngestInfo>   mIngestInfo;
    std::shared_ptr<Thread> mIngestThread;      // 拉取视频流的线程
    long long           mFrameIndex = 0;        // 视频帧的index

    AVFormatContext     *mFormatContext = NULL;
    AVStream            *mVideoStream = NULL;
    AVCodec             *mVideoCodec = NULL;
    AVCodecContext      *mVideoCodecContext = NULL;     // 解码器
    int                 mVideoStreamIndex = 0;
};

#endif
