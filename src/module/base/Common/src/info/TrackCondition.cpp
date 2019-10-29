#include <sstream>
#include "TrackCondition.h"

TrackCondition::TrackCondition(std::string abstractId, std::set<AbstractType> types, AbstractModel model, int frameRate, int faceCount)
{
    mAbstractId = abstractId;
    mAbstractType = types;
    mAbstractModel = model;
    mFrameRate = frameRate;
    mAbstractFaceCount = faceCount;
}

// 获取提取的ID
std::string TrackCondition::getAbstractId()
{
    return mAbstractId;
}

// 获取提取的类型(人体还是人脸)
std::set<AbstractType> TrackCondition::getAbstractType()
{
    return mAbstractType;
}

// 获取提取的模式(实时优先还是质量优先)
AbstractModel TrackCondition::getAbstractModel()
{
    return mAbstractModel;
}

// 获取视频的帧率
int TrackCondition::getFrameRate()
{
    return mFrameRate;
}

// 获取输出的人脸的张数
int TrackCondition::getAbstractFaceCount()
{
    return mAbstractFaceCount;
}

std::string TrackCondition::toString()
{
    std::stringstream buf;
    buf << "abstract id:" << mAbstractId << ", abstract type:";
    for (AbstractType type : mAbstractType)
    {
        buf << Common::getAbstraceTypeName(type) << ", ";
    }
    buf << "abstract model:" << Common::getAbstraceModelName(mAbstractModel) << ", video frame ratio:" << mFrameRate << ", abstract face count:" << mAbstractFaceCount;
    return buf.str();
}
