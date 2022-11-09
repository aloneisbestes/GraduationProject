#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <map>
#include "HttpBase.hpp"
#include "DataTypeBase.hpp"

class HttpRequest : public HttpBase {
private:
    using MapStr = std::map<std::string, std::string>;
    // 默认容量
    const int DefaultCapacity = 4096;

private:
    std::string m_url;          // 请求地址
    MapStr m_header;            // 请求头信息
    char *m_data;               // 请求的所有数据
    int m_start;                // 开始位置
    int m_capacity;             // 所有数据的容量
    DataTypeBase *m_content;    // 请求携带的参数

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
    // 获取请求携带的内容
    inline DataTypeBase *getContent() const { return m_content; }

public:
    /** 继承的虚函数和纯虚函数 **/
    bool run() override;

private:
    /** 内部私有处理函数 **/
    bool readData();    // 读取socket网络套接字上的数据
    bool parseRequestLine();    // 解析请求行
    bool parseRequestHeader();  // 解析请求体
    bool parseRequestContent(); // 解析请求携带的内容
};


#endif // __HTTP_REQUEST_H__