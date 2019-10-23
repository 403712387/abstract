#ifndef INGEST_INFO_H
#define INGEST_INFO_H
#include "Common.h"
#include "jsoncpp/json.h"
/*
 * 拉流的请求
 * request id是请求的ID，stream id是流的ID
 * 比如用户请求看device 为4的实时视频流，则stream ID可能为 device4tcp,在用户观看过程中，此stream id一直保持不变
 * request id用来标识一次拉流请求，比如ffmpeg正在拉流时，因为网络或者其他原因导致拉流失败，此时需要再次发送拉流请求
 * 这两次拉流stream id是一样的，但是request是不一样的
*/

class ABSTRACT_EXPORT IngestInfo
{
public:
    IngestInfo(std::string streamId, std::string streamUrl, int playDuration = 0);

    // 获取streamId
    std::string getStreamId();

    // 获取拉流的url地址
    std::string getStreamUrl();

    // 获取播放时长
    int getPlayDuration();

    // 请求的ID
    long long getRequestId();

    // 添加视频帧数
    void addFrameCount(int count = 1);
    long long getFrameCount();

    // 获取生成时间
    QDateTime getBirthday();

    virtual std::string toString();
    virtual Json::Value toJson();

protected:
    long long           mRequestId = 0;
    std::string         mStreamId;
    QDateTime           mBirthday = QDateTime::currentDateTime();
    long long           mFrameCount = 0;
    std::string         mStreamUrl;
    int                 mPlayDuration = 0;  // 播放的时长
};

#endif
