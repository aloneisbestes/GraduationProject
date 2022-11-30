#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <map>
#include "HttpBase.hpp"
#include "DataTypeBase.hpp"
#include "mmacro.hpp"

class HttpRequest : public HttpBase {
private:
    using MapStr = std::map<std::string, std::string>;
    // 默认容量
    const int DefaultCapacity = 4096;

    // 获取一行状态
    enum LineStatus {
        LineComplete=0,     // 获取一行完成
        LineFault,          // 获取一行错误
        LineIncomplete,     // 不完整的行，代表当前请求不完整
    };

    // 解析http状态
    enum ParseStatus {
        ParseLine,      // 解析请求行
        ParseHeader,    // 解析请求体
        ParseContent,   // 解析请求内容
        ParseComplete,  // 解析完成
    };

private:
    std::string m_url;          // 请求地址
    MapStr m_header;            // 请求头信息
    char *m_data;               // 请求的所有数据
    int m_start;                // 开始位置
    int m_size;                 // 当前数据大小
    int m_capacity;             // 所有数据的容量
    DataTypeBase *m_content;    // 请求携带的参数
    int m_mode;                 // 使用的是epoll的那种模式，如果传入-1表示没有使用epoll机制
    int m_now_idx;              // 当前所在数据data的index位置
    ParseStatus m_parse_state;  // http当前解析状态

public:
    /** 构造与析构 **/
    HttpRequest(int sockfd=-1, int mode=EPOLLIN_MODE);
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
    virtual int run() override;

private:
    /** 内部私有处理函数 **/
    int readData();    // 读取socket网络套接字上的数据
    ParseStatus parseRequestLine(char *line);    // 解析请求行
    ParseStatus parseRequestHeader(char *line);  // 解析请求体
    ParseStatus parseRequestContent(char *line); // 解析请求携带的内容
    LineStatus getLine();        // 获取请求数据的一行内容

private:
    bool dilatation();  // 扩容操作
};


#endif // __HTTP_REQUEST_H__