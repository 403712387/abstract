#include <string>
#include <QFileInfo>
#include <QDateTime>
#include "Log.h"
#include "Common.h"
#include "FaceDetect.h"
#include "FastFaceDetect.h"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "facedetectcnn.h"

#define DETECT_BUFFER_SIZE 0x20000

// 人脸检测
void fastFaceDetect()
{
    const std::string mClassName = "fastFaceDetect";
    std::string sourctPath = "/data/mars/picdata/background";
    std::string outputPath = "/data/mars/picdata/background_fastface/";
    Common::createPath(outputPath);

    int * pResults = NULL;
    unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);

    long long totalTime = 0;
    int imageCount = 0;
    std::vector<std::string> files = Common::getFilesInDirectory(sourctPath);
    for (std::string fileName : files)
    {
        // 初始化内存
        memset(pBuffer, 0, DETECT_BUFFER_SIZE);

        // 输出的文件名
        QFileInfo fileInfo(QString::fromStdString(fileName));
        std::string outputFile = outputPath + fileInfo.fileName().toStdString();

        // 人脸检测
        cv::Mat image = cv::imread(fileName);
        QDateTime beginTime = QDateTime::currentDateTime();
        pResults = facedetect_cnn(pBuffer, (unsigned char*)(image.ptr(0)), image.cols, image.rows, (int)image.step);
        QDateTime endTime = QDateTime::currentDateTime();

        // 计算耗时
        totalTime += endTime.toMSecsSinceEpoch() - beginTime.toMSecsSinceEpoch();
        ++imageCount;

        // 绘制矩形
        int angle = 0;
        for(int i = 0; i < (pResults ? *pResults : 0); i++)
        {
            short * p = ((short*)(pResults+1))+142*i;
            int x = p[0];
            int y = p[1];
            int w = p[2];
            int h = p[3];
            //int confidence = p[4];
            angle = p[5];

            cv::rectangle(image, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 2);
            cv::putText(image, std::to_string(angle), cv::Point(x, y), cv::FONT_HERSHEY_COMPLEX, 4, (255, 0, 0), 4);
        }

        // 保存图片
        std::vector<int> param;
        param.push_back(cv::IMWRITE_JPEG_QUALITY);
        param.push_back(85);
        cv::imwrite(outputFile, image, param);
    }
    free(pBuffer);
    pBuffer = NULL;
    LOG_I(mClassName, "total image:" << imageCount << ", total spend time:" << totalTime << "ms, avg:" << totalTime/imageCount << "ms");

}
