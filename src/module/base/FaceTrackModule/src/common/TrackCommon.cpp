#include <atomic>
#include "TrackCommon.h"

// 获取faceID
long long getFaceId()
{
    static std::atomic_llong beginId = {0};
    return ++beginId;
}
