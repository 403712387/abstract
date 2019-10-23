#include <sstream>
#include "IngestInfo.h"

IngestInfo::IngestInfo(std::string streamId, std::string streamUrl, int playDuration)
{
    mRequestId = Common::getSequenceId();
    mStreamId = streamId;
    mStreamUrl = streamUrl;
    mPlayDuration = playDuration;
}

// 获取streamId
std::string IngestInfo::getStreamId()
{
    return mStreamId;
}

// 获取拉流的url地址
std::string IngestInfo::getStreamUrl()
{
    return mStreamUrl;
}

// 获取播放时长
int IngestInfo::getPlayDuration()
{
    return mPlayDuration;
}

// 请求的ID
long long IngestInfo::getRequestId()
{
    return mRequestId;
}

// 添加视频帧数
void IngestInfo::addFrameCount(int count)
{
    mFrameCount += count;
}

long long IngestInfo::getFrameCount()
{
    return mFrameCount;
}

// 获取生成时间
QDateTime IngestInfo::getBirthday()
{
    return mBirthday;
}

std::string IngestInfo::toString()
{
    std::stringstream buf;
    buf << "stream url:" << mStreamUrl << ", play duration:" << mPlayDuration << "ms, " << "request id:" << mRequestId << ", birthday:" << mBirthday.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString() << ", frame count:" << mFrameCount;
    return buf.str();
}

 Json::Value IngestInfo::toJson()
 {
     Json::Value result;
     result["stream_url"] = mStreamUrl;
     result["duration"] = std::to_string(mPlayDuration) + "ms";
     result["request_id"] = Json::Int64(mRequestId);
     result["frame_count"] = Json::Int64(mFrameCount);
     result["birthday"] = mBirthday.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
     return result;
 }
