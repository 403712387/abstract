#include <sstream>
#include "IngestInfo.h"
#include "VideoFormat.h"
#include "FFmpegIngestMessage.h"

// ffmpeg拉取流的消息
FFmpegIngestMessage::FFmpegIngestMessage(std::shared_ptr<IngestInfo> ingestInfo, MessageTransType transType ,PriorityType priority, MessageDistriubite distribute)
    :BaseMessage(FFmpeg_Ingest_Message, transType, priority, distribute)
{
    mIngestInfo = ingestInfo;
}

std::shared_ptr<IngestInfo> FFmpegIngestMessage::getFFmpegIngestInfo()
{
    return mIngestInfo;
}

std::string FFmpegIngestMessage::toString()
{
    std::stringstream buf;
    buf << "ffmpeg ingest:" << mIngestInfo->toString() << ", " << BaseMessage::toString();
    return buf.str();
}

/*
 * ffmpeg拉取流的回应
 */
FFmpegIngestResponse::FFmpegIngestResponse(std::shared_ptr<VideoFormat> &format, std::shared_ptr<BaseMessage> message, std::shared_ptr<Error> error)
    :BaseResponse(message, error)
{
    mVideoFormat = format;
}

// 获取视频流的格式
std::shared_ptr<VideoFormat> FFmpegIngestResponse::getVideoFormat()
{
    return mVideoFormat;
}

std::string FFmpegIngestResponse::toString()
{
    std::stringstream buf;
    if (NULL != mVideoFormat.get())
    {
      buf << "video format:" << mVideoFormat->toString() ;
    }
    else
    {
        buf << "video format is NULL";
    }
    buf << ", " << BaseResponse::toString();
    return buf.str();
}
