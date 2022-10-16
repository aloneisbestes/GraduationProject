#ifndef __HTTP_H__
#define __HTTP_H__

/**
 * @file http.h
 * @author 910319432@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 * @brief
 * 本头文件主要实现对http请求头的解析以及对http响应头的添加
 * 目前只识别http的：GET,POST,DELETE,PUT
 */

#include <string>
#include <map>

using std::string;
using std::map;

namespace HttpProtocol{

/**
 * @brief 
 * http的请求类型定义，目前只识别：GET,POST,DELETE,PUT
 */
enum HttpType {
    GET=1,          // get请求
    POST,           // post请求
    DELETE,         // delete请求
    PUT,            // put 请求
};

/**
 * @brief 
 * http 的请求头
 */
class HttpRequest{ 
private:
    // 定义当前解析的是请求行还是请求头还是请求体
    enum StateLine {    // 状态行，表示当前是请求行还是其他两种
        REQUEST_LINE,   // 请求行
        REQUEST_HEAD,   // 请求头
        REQUEST_BODY,   // 请求体
        REQUEST_END,    // 结束
    };

    enum LineRet {      // 获取一行的结果
        LINE_BAD,       // 错误/坏的请求
        LINE_OPEN,      // 请求不完整
        LINE_OK,        // 一行获取完成    
    };

    enum ParseState {   // 当前的解析状态
        PARSE_OK,       // 解析完成
        PARSE_NO,       // 还没有解析完成
        PARSE_BAD,      // 错误的解析
    };

private:
    HttpType m_http_type;                   // http的请求类型
    string m_req_content;                   // htpp的请求类容
    string m_req_url;                       // 请求的url
    string m_req_uri;                       // 请求的uri
    string m_req_version;                   // 获取http版本
    map<string, string> m_req_arg;          // 请求携带的参数
    map<string, string> m_req_head_field;   // 请求头字段
    size_t m_content_size;                  // 请求内容的长度
    char *m_req;                            // http请求
    size_t m_req_capacity;                  // http请求的总容量
    size_t m_req_size;                      // http请求总长度
    int m_error;                            // 错误码
    StateLine m_parse_state;                // 当前解析的状态
    LineRet m_line_state;                   // 获取一行的状态
    int m_start_idx;                        // 一行的开始位置
    int m_parse_idx;                        // 解析的当前位置
    int m_burst_mode;                       // epoll的模式，默认使用EPOLLIN


public:
    /** 构造与析构 **/
    HttpRequest()=delete;   // 删除默认构造函数
    HttpRequest(const HttpRequest &req)=delete; // 删除拷贝构造函数
    HttpRequest(const char *req);
    HttpRequest(int sockfd, int burst_mode);
    ~HttpRequest();

public:
    /** 成员函数 **/
    // 获取请求类型
    HttpType getHttpType() const { return m_http_type; }
    // 获取请求类容
    const string &getReqContent() const { return m_req_content; }
    // 获取请求的url 
    const string &getReqUrl() const { return m_req_url; }
    // 获取请求携带的参数
    const map<string, string> &getReqArgs() const { return m_req_arg; }
    // 获取请求内容的长度
    size_t getContentSize() const { return m_content_size; }
    // 获取http版本
    const string &getVersion() const { return m_req_version; }
    // 获取请求头字段信息
    const map<string, string> &getHeadFedld() const { return m_req_head_field; }
    // 开始解析
    int start();

private:
    /** 私有成员函数 **/
    // 解析 http 请求行
    bool parseHttpRequestLine(char *request);
    // 解析 http 请求头
    bool parseHttpRequestHead(char *request);
    // 解析 http 请求内容
    bool parseHttpRequestContent(char *request);
    // 解析主进程, 如果成功返回0，失败返回错误码，错误码从1开始
    int parseMain();
    // 获取一行信息
    LineRet getLine();
};

/**
 * @brief 
 * http 的响应头
 */
class HttpResponse {
private:
    // string m_response_data;                         // 响应数据
    char *m_response_data;              // 响应数据
    size_t m_response_size;             // 响应数据的当前大小
    size_t m_response_capacity;         // 响应数据的容量

public:
    /** 构造和析构 **/
    HttpResponse(int capacity=1024);
    ~HttpResponse();

public:
    /** 公有函数 **/
    // 添加响应行
    bool addResponseHeader(const char *version, int http_code, const char *state_describe);
    // 添加响应头字段
    bool addHeaderField(const char *key, const char *value);
    bool addHeaderField(const char *key, int value);
    // 添加响应体
    bool addHeaderContent(const char *content);
    bool addHeaderContent(const string &content);
    // 获取响应数据
    const char *getResponseData() const { return m_response_data; }
    // 获取响应数据长度
    size_t getResponseDataSize() const { return m_response_size; }

private:
    /** 私有函数 **/
    bool dilatation();
};

/**
 * @brief 
 * http 协议
 */
class Http {
private:
    HttpRequest *m_request;      // http请求
    HttpResponse *m_response;    // http响应
    int m_sockfd;                // socket fd

public:
    /** 构造与析构 **/
    Http()=delete;                  // 不允许默认默认构造
    Http(const Http &http)=delete;  // 不允许拷贝构造
    Http(int fd, int burst_mode);   // 传入socket套接字，在内部读取数据
    Http(int fd, const char *req_data);  // 传入socket套接字及从套接字中读取的数据
    ~Http();

public:
    /** 请求成员函数 **/
    // 获取请求类型
    HttpType getHttpType() const { return m_request->getHttpType(); }
    // 获取请求类容
    const string &getReqContent() const { return m_request->getReqContent(); }
    // 获取请求的url 
    const string &getReqUrl() const { return m_request->getReqUrl(); }
    // 获取请求携带的参数
    const map<string, string> &getReqArgs() const { return m_request->getReqArgs(); }
    // 获取请求内容的长度
    size_t getContentSize() const { return m_request->getContentSize(); }
    // 获取版本号
    const string &getVersion() const { return m_request->getVersion(); }

    /** 响应成员函数 **/
    // 添加响应行
    bool addResponseHeader(const char *version, int http_code, const char *state_describe) { return m_response->addResponseHeader(version, http_code, state_describe); }
    // 添加响应头字段
    bool addHeaderField(const char *key, const char *value) { return m_response->addHeaderField(key, value); }
    bool addHeaderField(const char *key, int value) { return m_response->addHeaderField(key, value); }
    // 添加响应体
    bool addHeaderContent(const char *content) { return m_response->addHeaderContent(content); }
    bool addHeaderContent(const string &content) { return m_response->addHeaderContent(content); }
    // 获取响应数据
    const char *getResponseData() const { return m_response->getResponseData(); }
    // 获取响应数据长度
    size_t getResponseDataSize() const { return m_response->getResponseDataSize(); }

    // 开始解析
    int parseRequest() { return m_request->start(); }
    // 发送response
    int sendResponse();
};

}

#endif // __HTTP_H__