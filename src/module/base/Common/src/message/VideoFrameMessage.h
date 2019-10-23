#ifndef VIDEO_FRAME_MESSAGE_H
#define VIDEO_FRAME_MESSAGE_H
#include "BaseMessage.h"


/*
 * 视频帧信息
*/
class VideoFrameInfo;
class ABSTRACT_EXPORT VideoFrameMessage : public BaseMessage
{
public:
    VideoFrameMessage(std::shared_ptr<VideoFrameInfo> frame, MessageTransType transType = Async_Trans_Message,
                      PriorityType priority = Priority_Third, MessageDistriubite distribute = Message_Unicast);

    // 获取视频帧信息
    std::shared_ptr<VideoFrameInfo> getVideoFrameInfo();

    virtual std::string toString();

private:
    std::shared_ptr<VideoFrameInfo>     mFrameInfo;
};

#endif
