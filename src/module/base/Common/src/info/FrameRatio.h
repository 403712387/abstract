#ifndef FRAME_RATIO_H
#define FRAME_RATIO_H
#include <string>
#include "Base.h"

/*
 * 视频的帧率
 */
class ABSTRACT_EXPORT FrameRatio
{
public:
    FrameRatio(int numerator, int denominator);

    // 获取帧率
    float getFrameRatio();

    std::string toString();

private:
    int         mNumerator = 0;
    int         mDenominator = 0;
};

#endif
