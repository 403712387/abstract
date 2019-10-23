#ifndef VIDEO_FRAME_INFO
#define VIDEO_FRAME_INFO
#include <string>
#include <memory>
#include "Common.h"

/*
 * 视频帧的信息
 */
class ABSTRACT_EXPORT VideoFrameInfo
{
public:
    VideoFrameInfo(std::string streamId, long long frameIndex, VideoFrameType frameType, std::shared_ptr<std::string> frameData, long long pts, long long dts);

    // 获取流ID
    std::string getStreamId();

    // 获取帧的index
    long long getFrameIndex();

    // 获取帧的类型
    VideoFrameType getFrameType();

    // 获取帧的内容
    std::shared_ptr<std::string> getFrameData();

    // 获取pts
    long long getPts();

    // 获取dts
    long long getDts();

    // 获取生成帧的时间
    QDateTime getBirthday();

    std::string toString();

private:
    std::string         mStreamId;
    long long           mFrameIndex = 0;
    VideoFrameType      mFrameType = Video_Frame_I;
    std::shared_ptr<std::string> mFrameData;
    QDateTime           mBirthday;
    long long           mPts = 0;
    long long           mDts = 0;
};

#endif
