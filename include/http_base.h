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
    HttpBase(const std::string &m_ip="", int m_port=-1);
    HttpBase(const HttpBase &http_base);
    ~HttpBase();

public:
    // 获取和设置ip
    const std::string &getIP() const { return m_ip; }
    void setIP(const std::string &ip) { m_ip = ip; }
    // 获取和设置端口
    int getPort() const { return m_port; }
    void setPort(int port) { m_port = port; }

public:
    virtual int run() = 0;
};

#endif // __HTTP_BASE_H__