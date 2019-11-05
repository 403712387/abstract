#include <malloc.h>
#include <QtCore/QDateTime>
#include "Log.h"
#include "JsonHelper.h"
#include "BaseMessage.h"
#include "StopAbstract.h"
#include "ConfigureInfo.h"
#include "AbstractCondition.h"
#include "jsoncpp/json.h"

std::string JsonHelper::mClassName = "JsonHelper";

// 解析服务控制
ServiceOperateType JsonHelper::parseServiceControl(std::string &info)
{
    ServiceOperateType result = Operate_Exit;
    if (info.empty())
    {
        return result;
    }

    Json::Reader reader;
    Json::Value readerRoot;
    bool ret = reader.parse(info, readerRoot);
    if (false == ret)
    {
        LOG_E(mClassName, "prase service control fail");
        return result;
    }

    try
    {
        // 解析type
        if (!readerRoot.isMember("type"))
        {
            return result;
        }

        std::string value = readerRoot["type"].asString();
        if ("exit" == value)
        {
            result = Operate_Exit;
        }
        else if ("reload" == value)
        {
            result = Operate_Reload;
        }
    }
    catch (...)
    {
        LOG_E(mClassName, "prase service control exception");
    }

    return result;
}

// 解析配置
bool JsonHelper::parseConfigure(std::string &info, std::shared_ptr<ConfigureInfo> configure)
{
    if (info.empty())
    {
        LOG_E(mClassName, "parse configure fail, configure is empty");
        return false;
    }

    Json::Reader reader;
    Json::Value readerRoot;
    bool ret = reader.parse(info, readerRoot);
    if (false == ret)
    {
        LOG_E(mClassName, "prase json info fail, info:" << info);
        return false;
    }

    try
    {
        // 日志设置
        std::string logKey = "log";
        if (readerRoot.isMember(logKey))
        {
            // 级别
            if (readerRoot[logKey].isMember("level"))
            {
                configure->setLogLevel(Common::getLogLevelFromString(readerRoot[logKey]["level"].asString()));
            }

            // 关键字
            if (readerRoot[logKey].isMember("keyword"))
            {
                configure->setLogKeyword(readerRoot[logKey]["keyword"].asString());
            }
        }

        // http设置
        std::string cgfService = "abstract_service";
        if (readerRoot.isMember(cgfService))
        {
            // http port
            if (readerRoot[cgfService].isMember("http_port"))
            {
                int httpPort = readerRoot[cgfService]["http_port"].asInt();
                configure->setHttpPort(httpPort);
            }
        }
    }
    catch (...)
    {
        LOG_E(mClassName, "prase configure exception");
        return false;
    }

    return true;
}

// 解析提取的条件
std::pair<std::shared_ptr<AbstractCondition>, std::shared_ptr<Error>> JsonHelper::parseAbstractCondition(std::string &info)
{
    std::pair<std::shared_ptr<AbstractCondition>, std::shared_ptr<Error>> result;
    if (info.empty())
    {
        result.second = Common::getError("info empty");
        return result;
    }

    Json::Reader reader;
    Json::Value readerRoot;
    bool ret = reader.parse(info, readerRoot);
    if (false == ret)
    {
        result.second = Common::getError("parse json fail");
        return result;
    }

    try
    {
        if (readerRoot.isMember("url"))
        {
            std::string url = readerRoot["url"].asString();
            std::set<AbstractType> types;
            AbstractModel model = Abstract_Quality_First;

            // 提取类型
            if (readerRoot.isMember("type"))
            {
                for (int i = 0; i < readerRoot["type"].size(); ++i)
                {
                    types.insert(Common::getAbstractTypeByName(readerRoot["type"][i].asString()));
                }
            }

            if (types.empty())
            {
                types.insert(Abstract_Face);
            }

            // 提取模式
            if (readerRoot.isMember("model"))
            {
                model = Common::getAbstractModelByName(readerRoot["model"].asString());
            }

            // 提取的张数
            int abstractCount = 2;
            if (readerRoot.isMember("abstract_count"))
            {
                abstractCount = readerRoot["abstract_count"].asInt();
            }

            result.first = std::make_shared<AbstractCondition>(url, types, model, abstractCount);
        }
        else
        {
            result.second = Common::getError("invalid json info");
        }
    }
    catch (...)
    {
        result.second = Common::getError("parse json exception");
        return result;
    }

    return result;
}

// 解析停止提取的条件
std::pair<std::shared_ptr<StopAbstract>, std::shared_ptr<Error>> JsonHelper::parseStopAbstract(std::string &info)
{
    std::pair<std::shared_ptr<StopAbstract>, std::shared_ptr<Error>> result;
    if (info.empty())
    {
        result.second = Common::getError("info empty");
        return result;
    }

    Json::Reader reader;
    Json::Value readerRoot;
    bool ret = reader.parse(info, readerRoot);
    if (false == ret)
    {
        result.second = Common::getError("parse json fail");
        return result;
    }

    try
    {
        std::string key = "abstract_id";
        std::string id;
        std::set<AbstractType> types;
        if (readerRoot.isMember(key))
        {
            id = readerRoot[key].asString();

            // 提取类型
            if (readerRoot.isMember("type"))
            {
                for (int i = 0; i < readerRoot["type"].size(); ++i)
                {
                    types.insert(Common::getAbstractTypeByName(readerRoot["type"][i].asString()));
                }
            }

            if (types.empty())
            {
                types.insert(Abstract_Face);
                types.insert(Abstract_Person);
            }

            result.first = std::make_shared<StopAbstract>(id, types);
        }
        else
        {
            result.second = Common::getError("invalid json info, not find key abstract_id");
        }
    }
    catch (...)
    {
        result.second = Common::getError("parse json exception");
        return result;
    }

    return result;
}

// 提取的ID转成json
Json::Value JsonHelper::abstraceIdToJson(std::string abstractId)
{
    Json::Value result;
    result["abstract_id"] = abstractId;
    return result;
}
