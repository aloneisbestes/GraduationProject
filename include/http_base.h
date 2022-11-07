#ifndef __HTTP_BASE_H__
#define __HTTP_BASE_H__

/**
 * @brief 
 * http解析基类
 */

#include <string>

class HttpBase{
private:
    std::string m_ip;   // 请求的ip地址
    int m_port;         // 请求的ip端口

public:
    /** 构造与析构 **/
    HttpBase(const std::string &ip="", int port=-1):m_ip(ip), m_port(port){}
    HttpBase(const HttpBase &http_base):m_ip(http_base.m_ip), m_port(http_base.m_port){}
    ~HttpBase(){}

public:
    // 获取ip
    inline const std::string &getIP() const { return m_ip; }
    // 获取端口
    inline int getPort() const { return m_port; }

public:
    virtual int run() = 0;
};

#endif // __HTTP_BASE_H__