#ifndef FFMPEG_INGEST_MANAGER_AGENT_H
#define FFMPEG_INGEST_MANAGER_AGENT_H
#include <QMap>
#include <mutex>
#include "Common.h"

class VideoFormat;
class IngestInfo;
class VideoFrameInfo;
class IngestProcessor;
class IngestInfo;
class FFmpegIngestManager;
class FFmpegIngestManagerAgent
{
    friend class IngestProcessor;
public:
    FFmpegIngestManagerAgent(FFmpegIngestManager *manager);

    // 初始化
    bool init();

    //  反初始化
    void uninit();

    // 拉取视频流
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> processIngest(std::shared_ptr<IngestInfo> ingest);

    // 停止拉流
    bool processStopIngest(std::shared_ptr<IngestInfo> ingest);

    // 设置FFmpeg的日志级别
    void setFFmpegLogLevel(std::string level);

private:
    // 初始化ffmpeg
    void initFFmpeg();

    // 反初始化ffmpeg
    void uninitFFmpeg();

    // 添加一个拉流请求
    bool addIngestProcessor(std::shared_ptr<IngestProcessor> processor);

    // 是否存在一个拉取流请求
    bool isExistIngestProcessor(long long requestId);

    // 删除一个拉流请求
    bool removeIngestProcessor(long long requestId);

    // 停止所有的拉流请求
    void removeAllIngestProcessor();

    // 根据拉流请求，查找对应的processor
    std::shared_ptr<IngestProcessor> getIngestProcessor(long long requestId);

    // 发送视频帧
    void sendVideoFrame(std::shared_ptr<VideoFrameInfo> frame);

    // 记录ffmpeg的日志
    void initFFmpegLog();

    // ffmpeg拉流异常
    void ingestException(std::shared_ptr<IngestInfo> ingestInfo, StopIngestReason reason);

    // 获取FFmpeg日志级别
    int getFFmpegLogLevelFromString(std::string level);
private:
    std::string         mClassName = "FFmpegIngestManagerAgent";
    FFmpegIngestManager *mManager = NULL;

    QMap<long long, std::shared_ptr<IngestProcessor>>    mMapProcessor;
    std::mutex          mMapProcessorMutex;
};

#endif
