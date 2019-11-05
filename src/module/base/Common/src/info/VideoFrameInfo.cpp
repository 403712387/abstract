#include <sstream>
#include "VideoFrameInfo.h"

VideoFrameInfo::VideoFrameInfo(std::string streamId, long long frameIndex, VideoFrameType frameType, std::shared_ptr<cv::Mat> frameMat, long long pts, long long dts)
{
    mStreamId = streamId;
    mFrameIndex = frameIndex;
    mFrameType = frameType;
    mFrameMat = frameMat;
    mPts = pts;
    mDts = dts;
}

// 获取流ID
std::string VideoFrameInfo::getStreamId()
{
    return mStreamId;
}

// 获取帧的index
long long VideoFrameInfo::getFrameIndex()
{
    return mFrameIndex;
}

// 获取帧的类型
VideoFrameType VideoFrameInfo::getFrameType()
{
    return mFrameType;
}

// 获取帧的内容
std::shared_ptr<cv::Mat> VideoFrameInfo::getFrameMat()
{
    return mFrameMat;
}

// 获取pts
long long VideoFrameInfo::getPts()
{
    return mPts;
}

// 获取dts
long long VideoFrameInfo::getDts()
{
    return mDts;
}

// 获取生成帧的时间
QDateTime VideoFrameInfo::getBirthday()
{
    return mBirthday;
}

std::string VideoFrameInfo::toString()
{
    std::stringstream buf;
    buf << "stream id:" << mStreamId << ", frame index:" << mFrameIndex << ", frame type:" << Common::getVideoFrameTypeName(mFrameType) << ", pts:" << mPts << ", dts:" << mDts << ", birthday:" << mBirthday.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    return buf.str();
}
