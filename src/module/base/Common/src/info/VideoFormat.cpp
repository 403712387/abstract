#include <sstream>
#include "FrameRatio.h"
#include "VideoFormat.h"

VideoFormat::VideoFormat(std::string streamId, VideoFormatType type, int width, int height, std::shared_ptr<FrameRatio> frameRatio)
{
    mStreamId = streamId;
    mVideoFormatType = type;
    mVideoWidth = width;
    mVideoHeight = height;
    mFrameRatio = frameRatio;
}

// 获取stream id
std::string VideoFormat::getStreamId()
{
    return mStreamId;
}

// 获取视频格式
VideoFormatType VideoFormat::getVideoFrameType()
{
    return mVideoFormatType;
}

// 获取宽
int VideoFormat::getVideoWidth()
{
    return mVideoWidth;
}

// 获取长
int VideoFormat::getVideoHeight()
{
    return mVideoHeight;
}

// 获取帧率
std::shared_ptr<FrameRatio> VideoFormat::getFrameRatio()
{
    return mFrameRatio;
}

std::string VideoFormat::toString()
{
    std::stringstream buf;
    buf << "stream id:" << mStreamId << ", video format type:" << Common::getVideoFormatTypeName(mVideoFormatType) << ", width:" << mVideoWidth << ", height:" << mVideoHeight << ", frame ratio:" << mFrameRatio->toString();
    return buf.str();
}

Json::Value VideoFormat::toJson()
{
    Json::Value result;
    result["stream_id"] = mStreamId;
    result["video_format"] = Common::getVideoFormatTypeName(mVideoFormatType);
    result["resolution"]["width"] = mVideoWidth;
    result["resolution"]["height"] = mVideoHeight;
    result["frame_ratio"] = std::to_string((int)mFrameRatio->getFrameRatio()) + "f/s";
    return result;
}
