#ifndef __HTTP_BASE_H__
#define __HTTP_BASE_H__

/**
 * @file HttpBase.hpp
 * @author aloneisbestes@gmail.com
 * @brief http请求和解析基类
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <string>

class HttpBase{
private:
    std::string m_ip;   // 请求的ip地址
    int m_port;         // 请求的ip端口

protected:
    int m_sockfd;       // 连接的socket套接字
    std::string m_version;  // http请求版本号

public:
    /** 构造与析构 **/
    HttpBase(int sockfd=-1, const std::string &ip="", int port=-1):m_sockfd(sockfd), m_ip(ip), m_port(port){}
    HttpBase(const HttpBase &http_base):m_sockfd(http_base.m_sockfd), m_ip(http_base.m_ip), m_port(http_base.m_port){}
    virtual ~HttpBase(){}

protected:
    // 设置连接的ip
    void setConnectIP(const std::string &ip) { m_ip = ip; }
    // 设置连接的端口
    void setConnectPort(int port) { m_port = port; }

public:
    // 获取ip
    inline const std::string &getIP() const { return m_ip; }
    // 获取端口
    inline int getPort() const { return m_port; }

public:
    virtual bool run() = 0;
};

#endif // __HTTP_BASE_H__