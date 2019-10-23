#include <sstream>
#include "StopAbstract.h"

StopAbstract::StopAbstract(std::string id, std::set<AbstractType> type)
{
    mAbstractId = id;
    mAbstractType = type;
}

// 获取提取的ID
std::string StopAbstract::getAbstractId()
{
    return mAbstractId;
}

// 获取提取的类型
std::set<AbstractType> StopAbstract::getAbstractType()
{
    return mAbstractType;
}

std::string StopAbstract::toString()
{
    std::stringstream buf;
    buf << "id:" << mAbstractId << ", type:";
    for (AbstractType type : mAbstractType)
    {
        buf << Common::getAbstraceTypeName(type) << ", ";
    }
    return buf.str();
}
