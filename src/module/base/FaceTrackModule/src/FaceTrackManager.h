#ifndef FACE_TRACK_MANAGER_H
#define FACE_TRACK_MANAGER_H
#include "Common.h"
#include "BaseProcess.h"
/*
 * 从视频帧中跟踪出来人脸/人体
*/
class FaceTrackManagerAgent;
class ABSTRACT_EXPORT FaceTrackManager : public BaseProcess
{
    friend class FaceTrackManagerAgent;
public:
    FaceTrackManager(MessageRoute *messageRoute);

protected:
    virtual bool init();
    virtual void beginWork();

    // 卸载模块
    virtual void uninit();

    // 处理消息的函数
    virtual std::shared_ptr<BaseResponse> onProcessMessage(std::shared_ptr<BaseMessage> &message);

    // 处理消息的回应
    virtual void onProcessResponse(std::shared_ptr<BaseResponse> &response);

    // 消息队列中没有消息的时候调用
    virtual void onIdle();

private:
    // 处理人脸位置的信息
    std::shared_ptr<BaseResponse> onProcessFacePositionMessage(std::shared_ptr<BaseMessage> &message);

    // 处理停止跟踪的消息
    std::shared_ptr<BaseResponse> onProcessStopTrackMessage(std::shared_ptr<BaseMessage> &message);

private:
    std::shared_ptr<FaceTrackManagerAgent>      mAgent;
};

#endif
