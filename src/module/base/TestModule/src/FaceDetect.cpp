#include <string>
#include <QFileInfo>
#include <QDateTime>
#include "Log.h"
#include "Common.h"
#include "FaceDetect.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

// 人脸检测
void faceDetect()
{
    const std::string mClassName = "faceDetect";
    std::string sourctPath = "/data/mars/picdata/background";
    std::string outputPath = "/data/mars/picdata/background_face/";
    Common::createPath(outputPath);

    // 加载模型
    std::string cascadeName = "./data/haarcascades/haarcascade_frontalface_alt.xml";
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

        // 人脸检测
        cv::Mat image = cv::imread(fileName);
        cv::Mat grayImage;
        QDateTime beginTime = QDateTime::currentDateTime();
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(grayImage, grayImage);
        std::vector<cv::Rect> faces;
        cascade.detectMultiScale(grayImage, faces, 1.1, 2, 0|cv::CASCADE_SCALE_IMAGE,cv::Size(30, 30));
        QDateTime endTime = QDateTime::currentDateTime();

        // 计算耗时
        totalTime += endTime.toMSecsSinceEpoch() - beginTime.toMSecsSinceEpoch();
        ++imageCount;

        // 绘制矩形
        for (cv::Rect faceRect : faces)
        {
            cv::rectangle(image, faceRect, cv::Scalar(0,0,255));
        }

        // 保存图片
        std::vector<int> param;
        param.push_back(cv::IMWRITE_JPEG_QUALITY);
        param.push_back(85);
        cv::imwrite(outputFile, image, param);
    }

    LOG_I(mClassName, "total image:" << imageCount << ", total spend time:" << totalTime << "ms, avg:" << totalTime/imageCount << "ms");
}
