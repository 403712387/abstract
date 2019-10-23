#ifndef TRACK_MANAGER_AGENT_H
#define TRACK_MANAGER_AGENT_H
#include "Common.h"

class TrackManager;
class TrackManagerAgent
{
public:
    TrackManagerAgent(TrackManager *manager);

    // 初始化
    bool init();

    // 反初始化
    bool uninit();

    // 开始跟踪
    bool startTrack(std::string abstractId, AbstractType type);

    // 停止跟踪
    bool stopTrack(std::string abstractId, AbstractType type);

private:
    std::string     mClassName = "TrackManagerAgent";
    TrackManager    *mManager = NULL;
};

#endif
