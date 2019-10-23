#ifndef TRACK_FACE_MANAGER_H
#define TRACK_FACE_MANAGER_H
#include "Common.h"
#include "BaseProcess.h"
/*
 * 从视频帧中跟踪出来人脸/人体
*/
class TrackManagerAgent;
class ABSTRACT_EXPORT TrackManager : public BaseProcess
{
public:
    TrackManager(MessageRoute *messageRoute);

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
    std::shared_ptr<TrackManagerAgent>      mAgent;
};

#endif
