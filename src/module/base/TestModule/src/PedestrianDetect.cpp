#include <string>
#include <QFileInfo>
#include <QDateTime>
#include "Log.h"
#include "Common.h"
#include "PedestrianDetect.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

class Detector
{
    enum Mode { Default, Daimler } m;
    cv::HOGDescriptor hog, hog_d;
public:
    Detector() : m(Default), hog(), hog_d(cv::Size(48, 96), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9)
    {
        hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
        hog_d.setSVMDetector(cv::HOGDescriptor::getDaimlerPeopleDetector());
    }
    void toggleMode() { m = (m == Default ? Daimler : Default); }
    std::string modeName() const { return (m == Default ? "Default" : "Daimler"); }
    std::vector<cv::Rect> detect(cv::InputArray img)
    {
        // Run the detector with default parameters. to get a higher hit-rate
        // (and more false alarms, respectively), decrease the hitThreshold and
        // groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
        std::vector<cv::Rect> found;
        if (m == Default)
        {
            hog.detectMultiScale(img, found, 0, cv::Size(8,8), cv::Size(32,32), 1.05, 2, false);
        }
        else if (m == Daimler)
        {
            hog_d.detectMultiScale(img, found, 0.5, cv::Size(8,8), cv::Size(32,32), 1.05, 2, true);
        }
        return found;
    }
    void adjustRect(cv::Rect & r) const
    {
        // The HOG detector returns slightly larger rectangles than the real objects,
        // so we slightly shrink the rectangles to get a nicer output.
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.07);
        r.height = cvRound(r.height*0.8);
    }
};

// 行人检测
void pedestrianDetect()
{
    const std::string mClassName = "pedestrianDetect";
    std::string sourctPath = "/data/mars/picdata/pedestrian";
    std::string outputPath = "/data/mars/picdata/pedestrian_result/";
    Common::createPath(outputPath);

    long long totalTime = 0;
    int imageCount = 0;
    Detector detector;
    std::vector<std::string> files = Common::getFilesInDirectory(sourctPath);
    for (std::string fileName : files)
    {
        // 输出的文件名
        QFileInfo fileInfo(QString::fromStdString(fileName));
        std::string outputFile = outputPath + fileInfo.fileName().toStdString();

        // 人体检测
        cv::Mat image = cv::imread(fileName);
        QDateTime beginTime = QDateTime::currentDateTime();
        std::vector<cv::Rect> pedestrians = detector.detect(image);
        QDateTime endTime = QDateTime::currentDateTime();

        // 计算耗时
        totalTime += endTime.toMSecsSinceEpoch() - beginTime.toMSecsSinceEpoch();
        ++imageCount;

        // 绘制矩形
        for (cv::Rect pedestrian : pedestrians)
        {
            cv::rectangle(image, pedestrian, cv::Scalar(0,0,255));
        }

        // 保存图片
        std::vector<int> param;
        param.push_back(cv::IMWRITE_JPEG_QUALITY);
        param.push_back(85);
        cv::imwrite(outputFile, image, param);
    }

    LOG_I(mClassName, "total image:" << imageCount << ", total spend time:" << totalTime << "ms, avg:" << totalTime/imageCount << "ms");
}
