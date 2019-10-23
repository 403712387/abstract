#ifndef FFMPEG_INGEST_MESSAGE_H
#define FFMPEG_INGEST_MESSAGE_H
#include "BaseMessage.h"

/*
 * ffmpeg拉流请求
 */
class VideoFormat;
class IngestInfo;
class ABSTRACT_EXPORT FFmpegIngestMessage : public BaseMessage
{
public:
    FFmpegIngestMessage(std::shared_ptr<IngestInfo> ingestInfo, MessageTransType transType = Async_Trans_Message,
                       PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    std::shared_ptr<IngestInfo> getFFmpegIngestInfo();
    virtual std::string toString();

private:
    std::shared_ptr<IngestInfo>      mIngestInfo;
};

/*
 * ffmpeg拉流的回应
 */
class ABSTRACT_EXPORT FFmpegIngestResponse : public BaseResponse
{
public:
    FFmpegIngestResponse(std::shared_ptr<VideoFormat> &format, std::shared_ptr<BaseMessage> message, std::shared_ptr<Error> error);

    // 获取视频流的格式
    std::shared_ptr<VideoFormat> getVideoFormat();

    virtual std::string toString();

private:
    std::shared_ptr<VideoFormat>        mVideoFormat;
};

#endif
