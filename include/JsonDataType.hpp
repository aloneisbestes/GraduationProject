#ifndef __JSON_DATA_TYPE_HPP__
#define __JSON_DATA_TYPE_HPP__

/**
 * @file JsonDataType.hpp
 * @author your name (you@domain.com)
 * @brief json数据类型
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <json/json.h>
#include <string>

#include "DataTypeBase.hpp"

class JsonData : public DataTypeBase {
private:
    Json::Value m_root;     // josn对象
    std::string m_jsonstr;  // json字符串

public:
    JsonData(int type);
    JsonData(int type, const std::string &jsonstr);
    JsonData(int type, const Json::Value &jsonobj);
    JsonData(const JsonData &json_data);
    virtual ~JsonData() {}

public:
    /** 重写父类方法 **/
    // 解析数据
    virtual void parseData() override;
    // 创建数据
    virtual void createData() override;
};

#endif // __JSON_DATA_TYPE_HPP__
