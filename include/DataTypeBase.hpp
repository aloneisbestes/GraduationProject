#ifndef __DATA_TYPE_BASE_HPP__
#define  __DATA_TYPE_BASE_HPP__

/**
 * @file DataTypeBase.hpp
 * @author aloneisbestes@gmail.com
 * @brief 定义不同的数据类型的抽象基类
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/**
 * @brief 
 * 所有数据类型的基类
 */
class DataTypeBase {
private:
    int m_type;     // 数据类型

public:
    // 构造与析构
    DataTypeBase(int type=-1):m_type(type) {}
    DataTypeBase(const DataTypeBase &data_type): m_type(data_type.m_type) {}
    virtual ~DataTypeBase() {}

    // 获取和设置数据类型
    int getDataType() const { return m_type; }
    void setDataType(int type) { m_type = type; }

public:
    virtual void parseData() = 0;
    virtual void createData() = 0;
};

#endif // __DATA_TYPE_BASE_HPP__