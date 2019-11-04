#include <sstream>
#include "Common.h"
#include "TrackFaceInfo.h"

TrackFaceInfo::TrackFaceInfo(std::string abstractId, long long faceId,  std::shared_ptr<cv::Mat> imageMat, QRect faceRect, int quality, QDateTime birthday)
{
    mAbstractId = abstractId;
    mFaceId = faceId;
    mImageMat = imageMat;
    mFaceRect = faceRect;
    mFaceQuality = quality;
    mBirthday = birthday;
}

// 获取abstract ID
std::string TrackFaceInfo::getAbstractId()
{
    return mAbstractId;
}

// 获取face id
long long TrackFaceInfo::getFaceId()
{
    return mFaceId;
}

// 获取图像Mat
std::shared_ptr<cv::Mat> TrackFaceInfo::getImageMat()
{
    return mImageMat;
}

// 获取人脸位置
QRect TrackFaceInfo::getFaceRect()
{
    return  mFaceRect;
}

// 获取人脸质量
int TrackFaceInfo::getFaceQuality()
{
    return mFaceQuality;
}

// 获取抓拍时间
QDateTime TrackFaceInfo::getBirthday()
{
    return mBirthday;
}

std::string TrackFaceInfo::toString()
{
    std::stringstream buf;
    buf << "abstract id:" << mAbstractId << "face id:" << mFaceId << ", face rect:" << Common::getRectString(mFaceRect) << ", face quality:" << mFaceQuality << ", birthday:" << mBirthday.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    return buf.str();
}
