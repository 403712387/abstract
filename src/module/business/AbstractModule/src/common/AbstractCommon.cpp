#include "AbstractCommon.h"

extern "C"
{
#include "ffmpeg/libavutil/adler32.h""
}

// 根据流的url,获取ID
std::string getAbstractId(std::string url)
{
    long long result = av_adler32_update(0, (const uint8_t *)url.data(), url.size());
    return std::to_string(result);
}
