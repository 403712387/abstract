#ifndef VIDEO_FORMAT_H
#define VIDEO_FORMAT_H
#include "Base.h"
#include "Common.h"
#include "jsoncpp/json.h"

/*
 * 视频的格式，比如编码格式，长，宽，帧率等
 */
class FrameRatio;
class ABSTRACT_EXPORT VideoFormat
{
public:
    VideoFormat(std::string streamId, VideoFormatType type, int width, int height, std::shared_ptr<FrameRatio> frameRatio);

    // 获取stream id
    std::string getStreamId();

    // 获取视频格式
    VideoFormatType getVideoFrameType();

    // 获取宽
    int getVideoWidth();

    // 获取长
    int getVideoHeight();

    // 获取帧率
    std::shared_ptr<FrameRatio> getFrameRatio();

    std::string toString();
    Json::Value toJson();

private:
    std::string         mStreamId;
    VideoFormatType     mVideoFormatType = Video_Format_H264;
    int                 mVideoWidth = 0;
    int                 mVideoHeight = 0;
    std::shared_ptr<FrameRatio> mFrameRatio;
};

#endif
