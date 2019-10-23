#ifndef ABSTRACE_MANAGER_AGENT_H
#define ABSTRACE_MANAGER_AGENT_H
#include <set>
#include <QMap>
#include <mutex>
#include <vector>
#include <memory>
#include "Error.h"
#include "Common.h"

class IngestInfo;
class VideoFormat;
class StopAbstract;
class AbstractTask;
class AbstractManager;
class AbstractCondition;
class AbstractTransaction;
class AbstractManagerAgent
{
public:
    AbstractManagerAgent(AbstractManager *manager);

    // 初始化
    bool init();

    // 反初始化
    bool uninit();

    // 开始提取人脸/人体信息
    std::pair<std::string, std::shared_ptr<Error>> startAbstract(std::shared_ptr<AbstractCondition> condition);

    // 停止提取人脸/人体信息
    bool stopAbstract(std::shared_ptr<StopAbstract> condition);

    // 拉流异常
    bool ingestException(std::shared_ptr<IngestInfo> ingestInfo);

private:
    // 处理添加任务
    std::pair<std::string, std::shared_ptr<Error>> processAddTask(std::string id, std::shared_ptr<AbstractCondition> condition);

    // 处理删除任务
    bool processRemoveTask(std::string id);

    // 处理更新任务
    std::pair<std::string, std::shared_ptr<Error>> processUpdateTask(std::shared_ptr<AbstractTask> task, std::set<AbstractType> types);

    // ffmpeg拉取流
    std::pair<std::shared_ptr<VideoFormat>, std::shared_ptr<Error>> ffmpegIngestStream(std::shared_ptr<IngestInfo> ingest, MessageTransType transType);

private:
    // 添加任务
    void addTask(std::shared_ptr<AbstractTask> &task);

    // 获取任务
    std::shared_ptr<AbstractTask> getTask(std::string id);

    // 是否存在任务
    bool isExistTask(std::string id);

    // 删除任务
    bool removeTask(std::string id);

    // 获取所有的任务
    std::vector<std::shared_ptr<AbstractTask>> getAllTask();

    // 发送开始跟踪消息
    void sendStartTrackMessage(std::string abstractId, std::set<AbstractType> types, AbstractModel model);

    // 发送停止跟踪消息
    void sendStopTrackMessage(std::string abstractId, std::set<AbstractType> types);

private:
    std::string         mClassName = "AbstractManagerAgent";
    AbstractManager     *mManager = NULL;

    QMap<std::string, std::shared_ptr<AbstractTask>>    mMapTask;       // 提取人脸/人体的任务
    std::mutex                                          mMapTaskLock;
};

#endif
