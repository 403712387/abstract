#ifndef FACE_TRACK_INFO_H
#define FACE_TRACK_INFO_H
#include <set>
#include "Common.h"
/*
 * 人脸跟踪的信息
 */
class ABSTRACT_EXPORT TrackCondition
{
public:
    TrackCondition(std::string abstractId, std::set<AbstractType> types, AbstractModel model, int frameRate, int faceCount);

    // 获取提取的ID
    std::string getAbstractId();

    // 获取提取的类型(人体还是人脸)
    std::set<AbstractType> getAbstractType();

    // 获取提取的模式(实时优先还是质量优先)
    AbstractModel getAbstractModel();

    // 获取视频的帧率
    int getFrameRate();

    // 获取输出的人脸的张数
    int getAbstractFaceCount();

    std::string toString();

private:
    std::string         mAbstractId;    // 提取的ID
    std::set<AbstractType>  mAbstractType;  // 提取的类型
    AbstractModel           mAbstractModel = Abstract_Quality_First; // 提取的模式
    int                     mFrameRate = 24;    // 帧率
    int                     mAbstractFaceCount = 2; // 提取出的人脸张数
};

#endif
