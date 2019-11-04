#include <QtCore/QDir>
#include <vector>
#include <atomic>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfoList>
#include "Common.h"
#include "opencv2/opencv.hpp"

#ifndef WIN32
#include <sys/prctl.h>
#endif

// 根据字符串获取日志级别
common_log::Log_Level Common::getLogLevelFromString(std::string level)
{
    common_log::Log_Level result = common_log::Log_Level_Info;
    static std::vector<std::string> levelString = {"verbose", "debug", "info", "warn", "error", "fatal"};
    static common_log::Log_Level logLevel[] = {common_log::Log_Level_Verbose, common_log::Log_Level_Debug,common_log::Log_Level_Info,
                                            common_log::Log_Level_Warn, common_log::Log_Level_Error, common_log::LOG_Level_Fatal};

    for (int i = 0; i < levelString.size(); ++i)
    {
        if (levelString[i] == level)
        {
            result = logLevel[i];
            break;
        }
    }

    return result;
}

// 获取日志级别的字符串
std::string Common::getLogLevelName(common_log::Log_Level level)
{
    std::string result = "info";
    switch(level)
    {
    case common_log::Log_Level_Verbose:
        result = "verbose";
        break;
    case common_log::Log_Level_Debug:
        result = "debug";
        break;
    case common_log::Log_Level_Info:
        result = "info";
        break;
    case common_log::Log_Level_Warn:
        result = "warn";
        break;
    case common_log::Log_Level_Error:
        result = "error";
        break;
    case common_log::LOG_Level_Fatal:
        result = "fatal";
        break;
    }
    return result;
}

// 获取服务控制类型的字符串
std::string Common::getServiceOperateTypeName(ServiceOperateType type)
{
    static std::vector<std::string> result = {"exit"};
    if (type < 0 || type >= result.size())
    {
        return result[0];
    }

    return result[type];
}

// 视频帧的类型
std::string Common::getVideoFrameTypeName(VideoFrameType type)
{
    static std::vector<std::string> result = {"unknown type", "I frame", "P frame", "B frame", "S-VOP", "switching Intra", "Switching Predicted", "BI type"};
    if (type < 0 || type >= result.size())
    {
        return result[0];
    }

    return result[type];
}

// 设置线程名字
void Common::setThreadName(std::string name)
{
#ifndef WIN32
    prctl(PR_SET_NAME, name.c_str());
#endif
}

// 生成一个唯一的ID
long long Common::getUniqueId()
{
    static long long beginTime = QDateTime::fromString("2018-12-25", "yyyy-MM-dd").toMSecsSinceEpoch();
    static long long current = QDateTime::currentMSecsSinceEpoch();
    static std::atomic_llong beginId = {(current - beginTime) << 20};
    return std::abs(++beginId);
}

// 生成一个sequence ID(sequence ID是从0开始自增的，Unique ID不是)
long long Common::getSequenceId()
{
    static std::atomic_llong beginId = {0};
    return std::abs(++beginId);
}

// 获取区域的字符串
std::string Common::getRectString(QRect rect)
{
    std::stringstream buf;
    buf << "left:" << rect.left() << ", top:" << rect.top() << ", width:" << rect.width() << ", height:" << rect.height();
    return buf.str();
}

// 获取端口的名字
std::string Common::getPortTypeName(PortType type)
{
    static std::vector<std::string> result = {"http"};
    if (type < 0 || type >= result.size())
    {
        return result[0];
    }

    return result[type];
}

// 获取视频格式的字符串
std::string Common::getVideoFormatTypeName(VideoFormatType type)
{
    static std::vector<std::string> result = {"h264", "h265"};
    if (type < 0 || type >= result.size())
    {
        return result[0];
    }

    return result[type];
}

// 获取提取的类型
std::string Common::getAbstraceTypeName(AbstractType type)
{
    static std::vector<std::string> result = {"face", "person"};
    if (type < 0 || type >= result.size())
    {
        return result[0];
    }

    return result[type];
}

// 根据名字，获取提取的类型
AbstractType Common::getAbstractTypeByName(std::string name)
{
    AbstractType result = Abstract_Face;
    static std::vector<std::string> typesName = {"face", "person"};
    static std::vector<AbstractType> types = {Abstract_Face, Abstract_Person};
    for (int i = 0; i < typesName.size(); ++i)
    {
        if (name == typesName[i])
        {
            result = types[i];
            break;
        }
    }
    return result;
}

// 获取提取的模式的字符串
std::string Common::getAbstraceModelName(AbstractModel type)
{
    static std::vector<std::string> result = {"realtime first", "quality first"};
    if (type < 0 || type >= result.size())
    {
        return result[0];
    }

    return result[type];
}

// 根据名字，获取提取的模式
AbstractModel Common::getAbstractModelByName(std::string name)
{
    AbstractModel result = Abstract_Quality_First;
    static std::vector<std::string> modelsName = {"realtime first", "quality first"};
    static std::vector<AbstractModel> models = {Abstract_Realtime_First, Abstract_Quality_First};
    for (int i = 0; i < modelsName.size(); ++i)
    {
        if (name == modelsName[i])
        {
            result = models[i];
            break;
        }
    }
    return result;
}

// 获取停止拉流原因的字符串
std::string Common::getStopIngestReasonString(StopIngestReason reason)
{
    static std::vector<std::string> result = {"end of file", "exception"};
    if (reason < 0 || reason >= result.size())
    {
        return result[0];
    }

    return result[reason];
}

// 获取目录下的所有文件
std::vector<std::string> Common::getFilesInDirectory(std::string directory)
{
    std::vector<std::string> result;
    QDir dir(QString::fromStdString(directory));

    // 目录是否存在
    if (!dir.exists())
    {
        return result;
    }

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot);
    for (QFileInfo fileInfo : fileInfoList)
    {
        if (fileInfo.isFile())      // 获取当前目录所有的文件
        {
            result.push_back(fileInfo.absoluteFilePath().toStdString());
        }
        else if (fileInfo.isDir())      // 获取子目录下的文件
        {
            std::vector<std::string> imageFiles = getFilesInDirectory(fileInfo.absoluteFilePath().toStdString());
            result.insert(result.end(), imageFiles.begin(), imageFiles.end());
        }
    }
    return result;
}

// 获取目录下的一个文件
std::string Common::getFileInDirectory(std::string directory)
{
    std::string result;
    QDir dir(QString::fromStdString(directory));

    // 目录是否存在
    if (!dir.exists())
    {
        return result;
    }

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot);
    for (QFileInfo fileInfo : fileInfoList)
    {
        if (fileInfo.isFile())      // 当前目录下的文件
        {
            result = fileInfo.absoluteFilePath().toStdString();
            return result;
        }
        else if (fileInfo.isDir())      // 获取子目录下的文件
        {
            result = getFileInDirectory(fileInfo.absoluteFilePath().toStdString());
            if (!result.empty())
            {
                break;
            }
        }
    }
    return result;
}

// 数据写入文件
bool Common::writeFile(std::string fileName, std::string &fileData)
{
    QFile file(QString::fromStdString(fileName));
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    if (file.write(fileData.c_str(), fileData.size()) < 0)
    {
        file.close();
        return false;
    }

    file.close();
    return true;
}


// 获取Mat
std::shared_ptr<cv::Mat> Common::getMat(std::string &data)
{
    std::shared_ptr<cv::Mat> result;
    cv::Mat image;
    try
    {
        cv::Mat bitStream(cv::Size((int)data.size(), 1), CV_8U, (void *)data.data());
        cv::imdecode(bitStream, cv::IMREAD_COLOR, &image);
    }
    catch (...)
    {
        return result;
    }

    result = std::make_shared<cv::Mat>(image);
    return result;
}


// 获取图片压缩率(检测的图片太大的话，把大图片压缩成小图片再处理, 把imageRect压缩为standRect)
int Common::getCompressRatio(QRect imageRect, QRect standRect)
{
    int result = 1;
    static std::vector<float> ratio = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f};

    // 如果比标准的小，则不用压缩
    if (standRect.width() > imageRect.width() && standRect.height() > imageRect.height())
    {
        return result;
    }

    // 压缩到标准以内
    float widthRatio = (float)imageRect.width() / (float)standRect.width();
    float heightRatio = (float)imageRect.height() / (float)standRect.height();
    float maxRatio = std::max(widthRatio, heightRatio);

    for (int i = 1; i < ratio.size(); ++i)
    {
        if (maxRatio < ratio[i])
        {
            result = (int)ratio[i];
            break;
        }
    }
    return result;
}

// 缩放mat
std::shared_ptr<cv::Mat> Common::resizeMat(cv::Mat *mat, QRect rect)
{
    std::shared_ptr<cv::Mat> result;
    cv::Mat outMat;
    cv::Size cvSize(rect.width(), rect.height());
    cv::resize(*mat, outMat, cvSize);
    result = std::make_shared<cv::Mat>(outMat);
    return result;
}

// 根据Mat,获取数据
std::string Common::getImageData(cv::Mat *mat, int compressRate)
{
    if (NULL == mat)
    {
        return "";
    }

    std::vector<int> param;
    param.push_back(cv::IMWRITE_JPEG_QUALITY);
    param.push_back(compressRate);

    std::vector<uchar> data;
    cv::imencode(".jpg", *mat, data, param);
    return std::string((char *)data.data(), data.size());
}

// 获取子Mat
std::shared_ptr<cv::Mat> Common::getSubMat(cv::Mat *mat, QRect rect)
{
    std::shared_ptr<cv::Mat> result;
    QRect imageRect(0, 0, mat->cols, mat->rows);
    if (!imageRect.contains(rect))
    {
        return result;
    }

    cv::Rect cvRect(rect.left(), rect.top(), rect.width(), rect.height());
    cv::Mat subMat = cv::Mat(*mat, cvRect);
    result = std::make_shared<cv::Mat>(subMat.clone());

    return result;
}

// 读文件
std::string Common::readFile(std::string fileName)
{
    std::string result;
    QFile file(QString::fromStdString(fileName));
    if (!file.exists())
    {
        return result;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        return result;
    }

    result = file.readAll().toStdString();
    file.close();
    return result;
}

// 获取易读的容量(比如，1.2G，100.7P这种)
std::string Common::getReadableCapacitySize(long long bytes)
{
    std::stringstream buf;
    buf.precision(1);
    buf.setf(std::ios::fixed);
    static std::vector<std::string> util = {"B","K", "M", "G", "T", "P", "E", "Z", "Y"};

    double capacity = (float)bytes;
    for (int i = 0; i < util.size(); ++i)
    {
        if (capacity < 1024)
        {
            buf << capacity << util.at(i);
            break;
        }

        capacity = capacity / 1024;
    }

    return buf.str();
}

// 获取http body tpye的字符串
std::string Common::getHttpTypeName(HttpBodyType type)
{
    static std::vector<std::string> result = {"text/json", "image/jpeg", "text/html", "application/octet-stream"};
    if (type < 0 || type >= result.size())
    {
        return result[0];
    }

    return result[type];
}

// 创建目录
bool Common::createPath(std::string path)
{
    QDir dir(QString::fromStdString(path));
    if (dir.exists())
    {
        return true;
    }

    bool ret = dir.mkpath(QString::fromStdString(path));
    return ret;
}

// 获取Error信息
std::shared_ptr<Error> Common::getError(std::string reason, long long code)
{
    std::shared_ptr<Error> result = std::make_shared<Error>(reason, code);
    return result;
}

// 没有错误
std::shared_ptr<Error> Common::noError()
{
    std::shared_ptr<Error> result;
    return result;
}

// 判断是否为有效的url
bool Common::isValidUrl(std::string url)
{
    if (url.empty())
    {
        return false;
    }
    return (url.find("/") != std::string::npos);
}

// 扩大roi矩形区域(第二个参数是倍数，如果是3倍，则宽，高为之前宽，高的3倍，面积是之前的9倍)
QRect Common::extenedRoiRect(QRect roiRect, int times, QRect imageRect)
{
    if (times <= 1)
    {
        return roiRect;
    }

    int newWidth = roiRect.width() * times;
    int newHeight = roiRect.height() * times;
    QRect result(roiRect.x() - (newWidth - roiRect.width())/2, roiRect.y() - (newHeight - roiRect.height())/2, newWidth, newHeight);
    result = result.intersected(imageRect);
    return result;
}

// 判断是否是本地文件
bool Common::isLocalFile(std::string url)
{
    return std::string::npos == url.find("://");
}
