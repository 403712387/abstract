#include <sstream>
#include "FrameRatio.h"

FrameRatio::FrameRatio(int numerator, int denominator)
{
    mNumerator = numerator;
    mDenominator = denominator;
}

// 获取帧率
float FrameRatio::getFrameRatio()
{
    if (mDenominator > -0.1 && mDenominator < 0.1)
    {
        return 0.0;
    }

    return (float)mNumerator / (float)mDenominator;
}

std::string FrameRatio::toString()
{
    std::stringstream buf;
    buf.precision(1);
    buf.setf(std::ios::fixed);
    buf << "numerator:" << mNumerator << ", denominator:" << mDenominator << ", frame ratio:" << getFrameRatio();
    return buf.str();
}
