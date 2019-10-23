#include "TrackManager.h"
#include "TrackManagerAgent.h"

TrackManagerAgent::TrackManagerAgent(TrackManager *manager)
{
    mManager = manager;
}

// 初始化
bool TrackManagerAgent::init()
{
    return true;
}

// 反初始化
bool TrackManagerAgent::uninit()
{
    return true;
}

// 开始跟踪
bool TrackManagerAgent::startTrack(std::string abstractId, AbstractType type)
{
    return true;
}

// 停止跟踪
bool TrackManagerAgent::stopTrack(std::string abstractId, AbstractType type)
{
    return true;
}
