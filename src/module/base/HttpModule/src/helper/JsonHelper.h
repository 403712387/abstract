#ifndef JSON_HELPER_H
#define JSON_HELPER_H
#include <map>
#include <vector>
#include "Common.h"
#include "jsoncpp/json.h"

class Error;
class StopAbstract;
class ConfigureInfo;
class AbstractCondition;
class JsonHelper
{
public:

    // 解析服务控制
    static ServiceOperateType parseServiceControl(std::string &info);

    // 解析配置
    static bool parseConfigure(std::string &info, std::shared_ptr<ConfigureInfo> configure);

    // 解析提取的条件
    static std::pair<std::shared_ptr<AbstractCondition>, std::shared_ptr<Error>> parseAbstractCondition(std::string &info);

    // 解析停止提取的条件
    static std::pair<std::shared_ptr<StopAbstract>, std::shared_ptr<Error>> parseStopAbstract(std::string &info);

    // 提取的ID转成json
    static Json::Value abstraceIdToJson(std::string abstractId);
private:
    static std::string        mClassName;
};

#endif
