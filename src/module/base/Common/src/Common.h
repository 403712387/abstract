#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H
#include <vector>
#include <string>
#include <QtCore/QRect>
#include <QtCore/QDateTime>
#include "Log.h"
#include "message/BaseMessage.h"

// 本机监听的端口类型
enum PortType
{
    Port_Http = 1,          // http端口
};

// 服务控制
enum ServiceOperateType
{
    Operate_Exit = 0,       // 退出服务
    Operate_Reload,         // 重新加载
};

// http的响应类型
enum HttpBodyType
{
    Http_Body_Json = 0,         // json格式
    Http_Body_Image = 1,        // 图片格式
    Http_Body_Html = 2,         // html格式
    Http_Body_File,             // 文件格式
};

// 视频格式
enum VideoFormatType
{
    Video_Format_H264,      // h264
    Video_Format_H265,      // h265
};

// 视频帧的类型(I/P/B帧)
enum VideoFrameType
{
    Video_Frame_NULL = 0, // 异常数据
    Video_Frame_I ,  // I帧
    Video_Frame_P,      // P帧
    Video_Frame_B,      // B帧
    Video_Frame_S,      // S(GMC)-VOP MPEG-4
    Video_Frame_SI,      // Switching Intra
    Video_Frame_SP,      // Switching Predicted
    Video_Frame_BI,      // BI type
};

// 停止拉流的原因
enum StopIngestReason
{
    Stop_End_Of_File = 0,       // 到文件结尾了
    Stop_Exception,             // 拉流过程中出现异常了
};

// 提取的类型
enum AbstractType
{
    Abstract_Face = 0,          // 提取人脸
    Abstract_Person,            // 提取人体 + 人脸
};

// 提取的模式（实时性优先还是效果优先）
enum AbstractModel
{
    Abstract_Realtime_First = 0,    // 实时性优先
    Abstract_Quality_First,         // 效果优先
};

namespace Common
{
    // 根据字符串获取日志级别
    ABSTRACT_EXPORT common_log::Log_Level getLogLevelFromString(std::string level);

    // 获取日志级别的字符串
    ABSTRACT_EXPORT std::string getLogLevelName(common_log::Log_Level level);

    // 获取服务控制类型的字符串
    ABSTRACT_EXPORT std::string getServiceOperateTypeName(ServiceOperateType type);

    // 视频帧的类型
    ABSTRACT_EXPORT std::string getVideoFrameTypeName(VideoFrameType type);

    // 设置线程名字
    ABSTRACT_EXPORT void setThreadName(std::string name);

    // 生成一个唯一的ID
    ABSTRACT_EXPORT long long getUniqueId();

    // 生成一个sequence ID(sequence ID是从0开始自增的，Unique ID不是)
    ABSTRACT_EXPORT long long getSequenceId();

    // 获取端口的名字
    ABSTRACT_EXPORT std::string getPortTypeName(PortType type);

    // 获取视频格式的字符串
    ABSTRACT_EXPORT std::string getVideoFormatTypeName(VideoFormatType type);

    // 获取提取的类型
    ABSTRACT_EXPORT std::string getAbstraceTypeName(AbstractType type);

    // 根据名字，获取提取的类型
    ABSTRACT_EXPORT AbstractType getAbstractTypeByName(std::string name);

    // 获取提取的模式的字符串
    ABSTRACT_EXPORT std::string getAbstraceModuleName(AbstractModel type);

    // 根据名字，获取提取的模式
    ABSTRACT_EXPORT AbstractModel getAbstractModelByName(std::string name);

    // 获取停止拉流原因的字符串
    ABSTRACT_EXPORT std::string getStopIngestReasonString(StopIngestReason reason);

    // 获取目录下的所有文件
    ABSTRACT_EXPORT std::vector<std::string> getFilesInDirectory(std::string directory);

    // 获取目录下的一个文件
    ABSTRACT_EXPORT std::string getFileInDirectory(std::string directory);

    // 数据写入文件
    ABSTRACT_EXPORT bool writeFile(std::string fileName, std::string &fileData);

    // 读文件
    ABSTRACT_EXPORT std::string readFile(std::string fileName);

    // 获取易读的容量(比如，1.2G，100.7P这种)
    ABSTRACT_EXPORT std::string getReadableCapacitySize(long long bytes);

    // 获取http body tpye的字符串
    ABSTRACT_EXPORT std::string getHttpTypeName(HttpBodyType type);

    // 创建目录
    ABSTRACT_EXPORT bool createPath(std::string path);

    // 获取Error信息
    ABSTRACT_EXPORT std::shared_ptr<Error> getError(std::string reason, long long code = 0);

    // 没有错误
    ABSTRACT_EXPORT std::shared_ptr<Error> noError();

    // 判断是否为有效的url
    ABSTRACT_EXPORT bool isValidUrl(std::string url);
}

#endif

