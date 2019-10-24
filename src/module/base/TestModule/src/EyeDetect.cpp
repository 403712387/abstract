#include <string>
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include "Log.h"
#include "Common.h"
#include "EyeDetect.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

// 眼睛检测
void eyeDetect()
{
    const std::string mClassName = "EyeDetect";
    std::string sourctPath = "/data/mars/picdata/face";
    std::string outputPath = "/data/mars/picdata/face_eye/";
    Common::createPath(outputPath);

    // 加载模型
    std::string cascadeName = "./data/haarcascades/haarcascade_eye.xml";
    cv::CascadeClassifier cascade;
    if (!cascade.load(cv::samples::findFile(cascadeName)))
    {
        LOG_E(mClassName, "load module " << cascadeName << " fail");
        return;
    }

    long long totalTime = 0;
    int imageCount = 0;
    std::vector<std::string> files = Common::getFilesInDirectory(sourctPath);
    for (std::string fileName : files)
    {
        // 输出的文件名
        QFileInfo fileInfo(QString::fromStdString(fileName));
        std::string outputFile = outputPath + fileInfo.fileName().toStdString();

        // 眼睛检测
        cv::Mat image = cv::imread(fileName);
        cv::Mat grayImage;
        QDateTime beginTime = QDateTime::currentDateTime();
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(grayImage, grayImage);
        std::vector<cv::Rect> eyes;
        cascade.detectMultiScale(grayImage, eyes, 1.1, 2, 0|cv::CASCADE_SCALE_IMAGE,cv::Size(30, 30));
        QDateTime endTime = QDateTime::currentDateTime();

        // 计算耗时
        totalTime += endTime.toMSecsSinceEpoch() - beginTime.toMSecsSinceEpoch();
        ++imageCount;

        // 绘制矩形
        for (cv::Rect eyeRect : eyes)
        {
            cv::rectangle(image, eyeRect, cv::Scalar(0,0,255));
        }

        // 保存图片
        std::vector<int> param;
        param.push_back(cv::IMWRITE_JPEG_QUALITY);
        param.push_back(85);
        cv::imwrite(outputFile, image, param);
    }
}
