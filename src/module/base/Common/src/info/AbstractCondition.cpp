#include <sstream>
#include "AbstractCondition.h"

AbstractCondition::AbstractCondition(std::string url, std::set<AbstractType> type, AbstractModel model, int abstractCount)
{
    mStreamUrl = url;
    mTypes = type;
    mModel = model;
    mAbstractCount = abstractCount;
}

// 获取流的url
std::string AbstractCondition::getStreamUrl()
{
    return mStreamUrl;
}

// 获取提取的类型（提取人脸还是人体）
std::set<AbstractType> AbstractCondition::getAbstractType()
{
    return mTypes;
}

// 更新提取的类型
void AbstractCondition::updateAbstractType(std::set<AbstractType> types)
{
    mTypes = types;
}

// 获取提取的模式(质量优先还是实时优先)
AbstractModel AbstractCondition::getAbstractModel()
{
    return mModel;
}

// 一个跟踪目标中提取图片的张数
int AbstractCondition::getAbstractCount()
{
    return mAbstractCount;
}

std::string AbstractCondition::toString()
{
    std::stringstream buf;
    buf << "stream url:" << mStreamUrl << ", abstract type:";
    for (AbstractType type : mTypes)
    {
        buf << Common::getAbstraceTypeName(type) << ", ";
    }
    buf << ", abstract module:" << Common::getAbstraceModelName(mModel) << ", abstract count:" << mAbstractCount << ", birthday:" << mBirthday.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    return buf.str();
}

Json::Value AbstractCondition::toJson()
{
    Json::Value result;
    result["url"] = mStreamUrl;
    int index = 0;
    for (AbstractType type : mTypes)
    {
        result["type"][index++] = Common::getAbstraceTypeName(type);
    }
    result["module"] = Common::getAbstraceModelName(mModel);
    result["abstract_count"] = mAbstractCount;
    result["birthday"] = mBirthday.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    return result;
}
