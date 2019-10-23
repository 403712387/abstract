#include <sstream>
#include "VideoFrameInfo.h"
#include "VideoFrameMessage.h"

VideoFrameMessage::VideoFrameMessage(std::shared_ptr<VideoFrameInfo> frame, MessageTransType transType ,PriorityType priority, MessageDistriubite distribute)
    :BaseMessage(Video_Frame_Message, transType, priority, distribute)
{
    mFrameInfo = frame;
}

// 获取视频帧信息
std::shared_ptr<VideoFrameInfo> VideoFrameMessage::getVideoFrameInfo()
{
    return mFrameInfo;
}

std::string VideoFrameMessage::toString()
{
    std::stringstream buf;
    buf << "frame info:" << mFrameInfo->toString() << ", " << BaseMessage::toString();
    return buf.str();
}
