#ifndef TRACK_FACE_INFO_H
#define TRACK_FACE_INFO_H
#include "Base.h"
#include <string>
#include <memory>
#include <QRect>
#include <QDateTime>

/*
 * 跟踪出来的人脸信息
*/
namespace cv
{
class Mat;
}
class ABSTRACT_EXPORT TrackFaceInfo
{
public:
    TrackFaceInfo(std::string abstractId, long long faceId, std::shared_ptr<cv::Mat> imageMat, QRect faceRect, int quality, QDateTime birthday);

    // 获取abstract ID
    std::string getAbstractId();

    // 获取face id
    long long getFaceId();

    // 获取图像Mat
    std::shared_ptr<cv::Mat> getImageMat();

    // 获取人脸位置
    QRect getFaceRect();

    // 获取人脸质量
    int getFaceQuality();

    // 获取抓拍时间
    QDateTime getBirthday();

    std::string toString();

private:
    std::string                     mAbstractId;
    long long                       mFaceId = 0;
    std::shared_ptr<cv::Mat>        mImageMat;
    QRect                           mFaceRect;
    int                             mFaceQuality;
    QDateTime                       mBirthday = QDateTime::currentDateTime();
};

#endif
