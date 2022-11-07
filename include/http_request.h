#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <map>
#include "http_base.h"

class HttpRequest : public HttpBase {
private:
    using MapStr = std::map<std::string, std::string>;

private:
    int m_sockfd;           // socket套接字
    std::string m_url;      // 请求地址
    std::string m_version;  // http请求版本号
    MapStr m_header;        // 请求头信息
    char *m_data;           // 请求的所有数据
    int m_start;            // 开始位置
    int m_capacity;         // 所有数据的容量

public:
    /** 构造与析构 **/
    HttpRequest(int sockfd=-1);
    HttpRequest(const HttpRequest &req);
    ~HttpRequest();

public:
    // 获取和设置sockfd
    inline int getSockfd() const { return m_sockfd; }
    inline void setSockfd(int fd) { m_sockfd = fd; }
    // 获取url
    inline const std::string &getUrl() const { return m_url; }
    // 获取version
    inline const std::string &getVersion() const { return m_version; }
    // 获取请求头信息
    inline const MapStr &getHeader() const { return m_header; }

public:
    /** 继承的虚函数和纯虚函数 **/
    int run() override;

private:
    /** 内部私有处理函数 **/
};


#endif // __HTTP_REQUEST_H__