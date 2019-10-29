#ifndef ABSTRACT_CONDITION_H
#define ABSTRACT_CONDITION_H
#include <set>
#include <QDateTime>
#include "Common.h"
#include "jsoncpp/json.h"

// 提取的条件
class ABSTRACT_EXPORT AbstractCondition
{
public:
    AbstractCondition(std::string url, std::set<AbstractType> type, AbstractModel model, int abstractCount);

    // 获取流的url
    std::string getStreamUrl();

    // 获取提取的类型（提取人脸还是人体）
    std::set<AbstractType> getAbstractType();

    // 更新提取的类型
    void updateAbstractType(std::set<AbstractType> types);

    // 获取提取的模式(质量优先还是实时优先)
    AbstractModel getAbstractModel();

    // 一个跟踪目标中提取图片的张数
    int getAbstractCount();

    std::string toString();
    Json::Value toJson();

private:
    std::string     mStreamUrl;
    std::set<AbstractType>  mTypes;
    AbstractModel  mModel = Abstract_Quality_First;
    int             mAbstractCount;     // 提取图片的张数
    QDateTime       mBirthday = QDateTime::currentDateTime();
};
#endif
