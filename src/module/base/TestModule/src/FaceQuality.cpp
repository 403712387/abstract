#include <string>
#include <QFileInfo>
#include <QDateTime>
#include <iostream>
#include "Log.h"
#include "Common.h"
#include "FaceQuality.h"
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

double blurDetect(Mat srcImage);

void faceQuality()
{
    const std::string mClassName = "FaceQuality";
    std::string sourctPath = "/data/mars/picdata/face";
    std::string outputPath = "/data/mars/picdata/face_quality/";
    Common::createPath(outputPath);

    long long totalTime = 0;
    int imageCount = 0;
    std::vector<std::string> files = Common::getFilesInDirectory(sourctPath);
    for (std::string fileName : files)
    {
        // 输出的文件名
        QFileInfo fileInfo(QString::fromStdString(fileName));
        std::string outputFile = outputPath + fileInfo.fileName().toStdString();

        // 质量检测
        cv::Mat image = cv::imread(fileName);
        QDateTime beginTime = QDateTime::currentDateTime();
        float quality = blurDetect(image);
        QDateTime endTime = QDateTime::currentDateTime();

        // 计算耗时
        totalTime += endTime.toMSecsSinceEpoch() - beginTime.toMSecsSinceEpoch();
        ++imageCount;

        // 绘制结果
        cv::putText(image, std::to_string(quality), cv::Point(0, image.rows / 2), cv::FONT_HERSHEY_COMPLEX, 1, (255, 0, 0), 1);

        // 保存图片
        std::vector<int> param;
        param.push_back(cv::IMWRITE_JPEG_QUALITY);
        param.push_back(85);
        cv::imwrite(outputFile, image, param);
    }
     LOG_I(mClassName, "total image:" << imageCount << ", total spend time:" << totalTime << "ms, avg:" << totalTime/imageCount << "ms");
}

//模糊检测，如果原图像是模糊图像，返回0，否则返回1
double blurDetect(Mat srcImage)
{

    Mat gray1;
    if (srcImage.channels() != 1)
    {
        //进行灰度化
        cvtColor(srcImage, gray1, CV_RGB2GRAY);
    }
    else
    {
        gray1 = srcImage.clone();
    }
    Mat tmp_m1, tmp_sd1;	//用来存储均值和方差
    double m1 = 0, sd1 = 0;
    //使用3x3的Laplacian算子卷积滤波
    Laplacian(gray1, gray1, CV_16S, 3);
    //归到0~255
    convertScaleAbs(gray1, gray1);
    //计算均值和方差
    meanStdDev(gray1, tmp_m1, tmp_sd1);
    m1 = tmp_m1.at<double>(0, 0);		//均值
    sd1 = tmp_sd1.at<double>(0, 0);		//标准差
#if 1
    return sd1*sd1;
#else
    //cout << "原图像：" << endl;
    cout << "均值: " << m1 << " , 方差: " << sd1*sd1 << endl;
    if (sd1*sd1 < 400)
    {
        cout << "原图像是模糊图像" << endl;
        return 0;
    }
    else
    {
        cout << "原图像是清晰图像" << endl;
        return 1;
    }
#endif
}
