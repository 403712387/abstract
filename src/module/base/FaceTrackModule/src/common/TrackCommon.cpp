#include <atomic>
#include "TrackCommon.h"

// 获取faceID
long long TrackCommon::getFaceId()
{
    static std::atomic_llong beginId = {0};
    return ++beginId;
}
