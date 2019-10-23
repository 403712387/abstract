#ifndef FFMPEG_INGEST_MANAGER
#define FFMPEG_INGEST_MANAGER
#include "BaseProcess.h"
/*
 * ffmpeg拉取视频流
*/
class IngestInfo;
class VideoFrameInfo;
class FFmpegIngestManagerAgent;
class ABSTRACT_EXPORT FFmpegIngestManager  : public BaseProcess
{
    friend class FFmpegIngestManagerAgent;
public:
    FFmpegIngestManager(MessageRoute *messageRoute);
protected:
    virtual bool init();
    virtual void beginWork();

    // 卸载模块
    virtual void uninit();

    // 处理消息的函数
    virtual std::shared_ptr<BaseResponse> onProcessMessage(std::shared_ptr<BaseMessage> &message);

    // 处理消息的回应
    virtual void onProcessResponse(std::shared_ptr<BaseResponse> &response);

private:
    // 处理拉取视频流的请求
    std::shared_ptr<BaseResponse> onProcessFFmpegIngestMessage(std::shared_ptr<BaseMessage> &message);

    // 处理停止拉取视频流的请求
    std::shared_ptr<BaseResponse> onProcessStopIngestMessage(std::shared_ptr<BaseMessage> &message);

    // 下发的配置信息
    std::shared_ptr<BaseResponse> onProcessConfigureMessage(std::shared_ptr<BaseMessage> &message);

    // 发送视频帧
    void sendVideoFrame(std::shared_ptr<VideoFrameInfo> frame);

    // 拉流异常或停止，发送消息通知业务模块
    void sendIngestStopMessage(std::shared_ptr<IngestInfo> ingestInfo, StopIngestReason reason);
private:
    std::shared_ptr<FFmpegIngestManagerAgent>   mAgent;
};

#endif
