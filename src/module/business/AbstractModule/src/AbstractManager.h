#ifndef ABSTRACT_MANAGER_H
#define ABSTRACT_MANAGER_H
#include "Common.h"
#include "BaseProcess.h"
/*
 * 从视频中提取出来人脸的业务模块
*/
class TrackCondition;
class IngestInfo;
class AbstractManagerAgent;
class ABSTRACT_EXPORT AbstractManager : public BaseProcess
{
    friend class AbstractManagerAgent;
public:
    AbstractManager(MessageRoute *messageRoute);

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
    // 处理开始提取人脸/人体的消息
    std::shared_ptr<BaseResponse> onProcessAbstractStartMessage(std::shared_ptr<BaseMessage> &message);

    // 处理停止提取人脸/人体
    std::shared_ptr<BaseResponse> onProcessAbstractStopMessage(std::shared_ptr<BaseMessage> &message);

    // 处理拉流异常的消息
    std::shared_ptr<BaseResponse> onProcessIngestExceptionMessage(std::shared_ptr<BaseMessage> &message);

    // 处理ffmpeg拉流的回应
    void onProcessFFmepgIngestResponse(std::shared_ptr<BaseResponse> &response);

    // 停止拉流
    bool sendStopIngestStreamMessage(std::shared_ptr<IngestInfo> ingest);

    // 发送开始跟踪消息
    void sendStartTrackMessage(std::shared_ptr<TrackCondition> trackInfo);

    // 发送停止跟踪消息
    void sendStopTrackMessage(std::string abstractId, AbstractType type);
private:
    std::shared_ptr<AbstractManagerAgent>    mAgent;
};

#endif
