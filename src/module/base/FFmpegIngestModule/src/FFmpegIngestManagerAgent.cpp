#include "IngestInfo.h"
#include "IngestProcessor.h"
#include "IngestInfo.h"
#include "FFmpegIngestManager.h"
#include "FFmpegIngestManagerAgent.h"
extern "C"
{
#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libavdevice/avdevice.h"
#include "ffmpeg/libavformat/avformat.h"
#include "ffmpeg/libavfilter/avfilter.h"
#include "ffmpeg/libavutil/avutil.h"
#include "ffmpeg/libswscale/swscale.h"
#include "ffmpeg/libavutil/log.h"
}
#ifdef WIN32
#define snprintf _snprintf;
#endif

void writeFFmpegLog(void* avcl, int level, const char *fmt, va_list argList)
{
    char buffer[1024] = {0};
    snprintf(buffer, sizeof(buffer), fmt, argList);

    std::string key = "FFmpeg";
    switch(level)
    {
    case AV_LOG_FATAL:
    case AV_LOG_QUIET:
    case AV_LOG_PANIC:
        LOG_F(key, buffer);
        break;
    case AV_LOG_ERROR:
        LOG_E(key, buffer);
        break;
    case AV_LOG_WARNING:
        LOG_W(key, buffer);
        break;
    case AV_LOG_INFO:
        LOG_I(key, buffer);
        break;
    case AV_LOG_VERBOSE:
        LOG_V(key, buffer);
        break;
    case AV_LOG_DEBUG:
        LOG_D(key, buffer);
        break;
    case AV_LOG_TRACE:
        LOG_V(key, buffer);
        break;
    default:
        LOG_I(key, buffer);
        break;
    }
}

FFmpegIngestManagerAgent::FFmpegIngestManagerAgent(FFmpegIngestManager *manager)
{
    mManager = manager;
}

// 初始化
bool FFmpegIngestManagerAgent::init()
{
    initFFmpeg();
    return true;
}

//  反初始化
void FFmpegIngestManagerAgent::uninit()
{
    uninitFFmpeg();
}

// 初始化ffmpeg
void FFmpegIngestManagerAgent::initFFmpeg()
{
    initFFmpegLog();
    avcodec_register_all();
    av_register_all();
    avformat_network_init();
    LOG_I(mClassName, "init ffmpage done");
}

// 记录ffmpeg的日志
void FFmpegIngestManagerAgent::initFFmpegLog()
{
    av_log_set_callback(writeFFmpegLog);
}

// 反初始化ffmpeg
void FFmpegIngestManagerAgent::uninitFFmpeg()
{
    LOG_I(mClassName, "uninit ffmpage done");
}

// ffmpeg停止拉流
void FFmpegIngestManagerAgent::ingestException(std::shared_ptr<IngestInfo> ingestInfo, StopIngestReason reason)
{
    std::shared_ptr<IngestProcessor> processor = getIngestProcessor(ingestInfo->getRequestId());
    if (NULL == processor.get())
    {
        LOG_E(mClassName, "stop ingest fail, not find process, intest info:" << ingestInfo->toString());
        return;
    }
    LOG_I(mClassName, "stop ingest stream, ingest info:" << ingestInfo->toString() << ", reason:" << Common::getStopIngestReasonString(reason));

    // 发送停止拉流的消息
    mManager->sendIngestStopMessage(ingestInfo, reason);
}

// 拉取视频流
std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> FFmpegIngestManagerAgent::processIngest(std::shared_ptr<IngestInfo> ingest)
{
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> result;

    // 已经正在拉流
    if (isExistIngestProcessor(ingest->getRequestId()))
    {
        std::shared_ptr<IngestProcessor> processor = getIngestProcessor(ingest->getRequestId());
        result.first = processor->getVideoFormat();
        return result;
    }

    // 创建processor
    std::shared_ptr<IngestProcessor> processor = std::make_shared<IngestProcessor>(this, ingest);
    addIngestProcessor(processor);
    result = processor->openStream();
    return result;
}

// 停止拉流
bool FFmpegIngestManagerAgent::processStopIngest(std::shared_ptr<IngestInfo> ingest)
{
    // 不存在对应的processor(可能不是ffmpeg拉流)
    if (!isExistIngestProcessor(ingest->getRequestId()))
    {
        return true;
    }

    std::shared_ptr<IngestProcessor> processor = getIngestProcessor(ingest->getRequestId());
    processor->closeStream();

    // 删除流
    removeIngestProcessor(ingest->getRequestId());
    return true;
}

// 设置FFmpeg的日志级别
void FFmpegIngestManagerAgent::setFFmpegLogLevel(std::string level)
{
    av_log_set_level(getFFmpegLogLevelFromString(level));
}

// 添加一路流
bool FFmpegIngestManagerAgent::addIngestProcessor(std::shared_ptr<IngestProcessor> processor)
{
    std::unique_lock<std::mutex> autoLocker(mMapProcessorMutex);

    // 如果已经存在，则不创建
    std::shared_ptr<IngestInfo> ingest = processor->getIngestInfo();
    if (mMapProcessor.contains(ingest->getRequestId()))
    {
        LOG_I(mClassName, "process already exist, ingest info:" << ingest->toString());
        return true;
    }

    LOG_I(mClassName, "add ingest process, ingest info:" << ingest->toString());
    mMapProcessor.insert(ingest->getRequestId(), processor);
    return true;
}

// 是否存在一路流
bool FFmpegIngestManagerAgent::isExistIngestProcessor(long long requestId)
{
    std::unique_lock<std::mutex> autoLocker(mMapProcessorMutex);
    return mMapProcessor.contains(requestId);
}

// 删除一路流
bool FFmpegIngestManagerAgent::removeIngestProcessor(long long requestId)
{
    std::unique_lock<std::mutex> autoLocker(mMapProcessorMutex);
    if (!mMapProcessor.contains(requestId))
    {
        LOG_W(mClassName, "remove ingest processor fail, not find ingest processor, request id:" << requestId);
        return false;
    }

    mMapProcessor.remove(requestId);
    LOG_I(mClassName, "remove ingest processor, request id:" << requestId);
    return true;
}

// 根据拉流请求，查找对应的processor
std::shared_ptr<IngestProcessor> FFmpegIngestManagerAgent::getIngestProcessor(long long requestId)
{
    std::shared_ptr<IngestProcessor> result;
    std::unique_lock<std::mutex> autoLocker(mMapProcessorMutex);
    if (!mMapProcessor.contains(requestId))
    {
        return result;
    }

    result = mMapProcessor.value(requestId);
    return result;
}

// 发送视频帧
void FFmpegIngestManagerAgent::sendVideoFrame(std::shared_ptr<VideoFrameInfo> frame)
{
    mManager->sendVideoFrame(frame);
}

// 获取FFmpeg日志级别
int FFmpegIngestManagerAgent::getFFmpegLogLevelFromString(std::string level)
{
    int result = AV_LOG_INFO;
    static std::vector<std::string> levelString = {"fatal", "fatal", "error", "warn", "info", "debug", "verbose"};
    static int logLevel[] = {AV_LOG_PANIC, AV_LOG_FATAL, AV_LOG_ERROR, AV_LOG_WARNING, AV_LOG_INFO, AV_LOG_DEBUG, AV_LOG_TRACE};

    for (int i = 0; i < levelString.size(); ++i)
    {
        if (levelString[i] == level)
        {
            result = logLevel[i];
            break;
        }
    }

    return result;
}
