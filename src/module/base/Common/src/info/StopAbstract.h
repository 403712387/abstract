#ifndef STOP_ABSTRACT_H
#define STOP_ABSTRACT_H
#include <set>
#include "Common.h"

/*
 * 停止提取人脸/人体信息
 */
class ABSTRACT_EXPORT StopAbstract
{
public:
    StopAbstract(std::string id, std::set<AbstractType> type);

    // 获取提取的ID
    std::string getAbstractId();

    // 获取提取的类型
    std::set<AbstractType> getAbstractType();

    std::string toString();

private:
    std::string         mAbstractId;
    std::set<AbstractType>  mAbstractType;
};

#endif
