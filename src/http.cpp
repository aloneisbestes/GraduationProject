#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "http.h"
#include "http_error.h"

using HttpProtocol::HttpRequest;
using HttpProtocol::HttpResponse;
using HttpProtocol::Http;

/**
 * @brief Construct a new Http Request:: Http Request object
 * 
 * @param req 
 */

HttpRequest::HttpRequest(const char *req) {
    // 初始化http请求数据的大小
    m_req_size = strlen(m_req);
    m_req_capacity = m_req_size;
    // 初始化请求数据
    m_req = new char[m_req_capacity];
    strcpy(m_req, req);
    // 初始化当前解析状态,初始化为解析请求行
    m_parse_state = StateLine::REQUEST_LINE;
    // 初始化一行开始的下标和解析的当前位置
    m_start_idx = 0;
    m_parse_idx = 0;
    // 初始化content的长度
    m_content_size = 0;
    // 初始化epoll模式
    m_burst_mode = 0;
}

HttpRequest::HttpRequest(int sockfd, int burst_mode=0) {
    /** 从 sockfd 中读取数据 **/
    // 初始化当前解析状态,初始化为解析请求行
    m_parse_state = StateLine::REQUEST_LINE;
    // 初始化一行开始的下标和解析的当前位置
    m_start_idx = 0;
    m_parse_idx = 0;
    // 初始化content的长度
    m_content_size = 0;
    // 初始化epoll模式
    m_burst_mode = burst_mode;
    // 初始化 m_req 的长度
    m_req_size = 0;

    m_req_capacity = 4096;
    m_req = (char *)malloc(sizeof(char) * m_req_capacity);
    memset(m_req, 0, m_req_capacity);

    // 开始读取数据
    int size = 0;
    if (m_burst_mode == 0) {
        // EPOLLIN
        size = recv(sockfd, m_req, m_req_capacity, 0);
        m_req_size = size;
    } else {
        while (true) {
            size = recv(sockfd, m_req+m_req_size, m_req_capacity-m_req_size, 0);
            if (size == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {  // 这两种情况是正常的，不是错误
                    m_req_size = 0;
                    break;
                }
            } else if (size == 0){
                m_req_size = 0;
                break;
            }
            m_req_size += size;
        }
    }

    printf("m_req: %s\n", m_req);
}

HttpRequest::~HttpRequest() {
    free(m_req);
}

// 解析 http 请求行
bool HttpRequest::parseHttpRequestLine(char *request) {
    // GET / HTTP/1.1\r\n
    /** 获取请求方式 **/
    char *move = strpbrk(request, " \t");
    if (!move) {
        // 如果 move 指针是一个空指针，则这个请求是一个错误的请求
        m_error = HttpErrorIdx::RequestBad;
        return false;
    }
    // 判断请求方式
    *move++ = '\0';
    char *method = request;
    if (strcasecmp(method, "GET") == 0) {
        m_http_type = HttpType::GET;
    } else if (strcasecmp(method, "POST") == 0) {
        m_http_type = HttpType::POST;
    } else if (strcasecmp(method, "PUT") == 0) {
        m_http_type = HttpType::PUT;
    } else if (strcasecmp(method, "DELETE") == 0) {
        m_http_type = HttpType::DELETE;
    } else {
        // 否则是一个错误的请求
        m_error = HttpErrorIdx::RequestBad;
        return false;
    }

    /** 获取 url **/
    move += strspn(move, " \t");  // 检索字符串 str1 中第一个不在字符串 str2 中出现的字符下标
    char *tmp_url = move;
    move = strpbrk(move, " \t");    // 找到版本号所在位置
    *move++ = '\0';
    

    /** 获取当前的http版本 **/
    m_req_version = move;

    /** 判断url是否有参数，针对 GET 请求 **/
    char *tmp;
    // 先保存uri
    m_req_uri = tmp_url;
    if (m_http_type == HttpType::GET && (tmp = strchr(tmp_url, '?')) != nullptr) {
        // 表示该请求路径有参数, 获取参数
        *tmp++ = '\0';
        // 先拷贝请求url
        m_req_url = tmp_url;
        // 获取请求路径中的参数
        char *tmp_args = nullptr;
        // 注: 后期可进行优化
        tmp_args = strchr(tmp, '&');
        do {
            // 判断tmp_args是否为空
            if (tmp_args != 0)
                *tmp_args++ = '\0';
            char *tmp_equal = strchr(tmp, '=');     // 找都等号  -> test=1&test1=2
            if (tmp_equal == nullptr) {
                m_req_arg.clear();
                m_error = HttpErrorIdx::GetRequestArgsError;    // get请求参数错误
                return false;
            }
            *tmp_equal = '\0';
            m_req_arg[tmp] = tmp_equal+1;
            // 更新tmp的值
            tmp = tmp_args;
            // 判断tmp是否为空
            if (tmp != nullptr)
                tmp_args = strchr(tmp, '&');
        } while (tmp != nullptr);
    } else {
        // 否则没有参数，直接拷贝url
        m_req_url = tmp_url;
    }

    /** 更新解析状态 **/
    m_parse_state = StateLine::REQUEST_HEAD;
    return true;
}

// 解析 http 请求头
bool HttpRequest::parseHttpRequestHead(char *request) {
    if (request[0] == '\0') {   // 表示头部解析解析
        if (m_content_size != 0) {
            m_parse_state = StateLine::REQUEST_BODY;  // 更新为解析请求内容
        } else {
            m_parse_state = StateLine::REQUEST_END; // 更新为解析结束
        }
    } else {
        // 将数据存储到 m_req_head_field
        char *move = strchr(request, ':');
        *move++ = '\0';
        // 保存键
        char *key = request;
        // 获取值
        move += strspn(move, " \t");  // 检索字符串 str1 中第一个不在字符串 str2 中出现的字符下标
        char *value = move;
        m_req_head_field[key] = value;

        // 处理content的长度
        if (strncasecmp(request, "Content-Length", 14) == 0) 
            m_content_size = std::atoi(value);
    }

    return true;
}

// 解析 http 请求内容
bool HttpRequest::parseHttpRequestContent(char *request) {
    if (m_req_size >= (m_content_size+m_parse_idx)) {  // 表示有请求内容
        // request[m_contetn_len] = '\0';
        m_req_content = request;
        m_parse_state = StateLine::REQUEST_END;
    } else {
        m_error = HttpErrorIdx::NoRequest;  // 不完整的请求
        return false;
    }

    return true;
}

// 解析主进程, 如果成功返回0，失败返回错误码，错误码从1开始
int HttpRequest::parseMain() {
    bool istrue = true;         // 是否是真, 开始初始化为真
    char *text = nullptr;       // 当前一行
    m_line_state = LineRet::LINE_OK;
    while ((m_parse_state != StateLine::REQUEST_END && m_line_state == LineRet::LINE_OK)) {
        // 如果不是获取内容，则可以获取一行，否则不用获取一行
        if (m_parse_state != StateLine::REQUEST_BODY)
            m_line_state = getLine();
        text = m_req + m_start_idx;
        m_start_idx = m_parse_idx;
        // 循环完成http请求解析
        switch (m_parse_state) {
            case StateLine::REQUEST_LINE:
                // 请求行
                istrue = parseHttpRequestLine(text);
                break;
            case StateLine::REQUEST_HEAD:
                // 请求头
                istrue = parseHttpRequestHead(text);
                break;
            case StateLine::REQUEST_BODY:
                // 请求体
                istrue = parseHttpRequestContent(text);
                break;
        }

        if (!istrue) {
            break;
        }
    }

    // 返回错误码，如果成功就返回0
    return istrue ? 0 : m_error;
}

// 获取一行
HttpRequest::LineRet HttpRequest::getLine() {
    char tmp = '\0';
    // GET / HTTP/1.1\r\n
    for (;m_parse_idx < m_req_size; m_parse_idx++) {
        tmp = m_req[m_parse_idx];
        if (tmp == '\r') {
            // 表示遇到一行的 \r 需要进行解析
            if ((m_parse_idx + 1) > m_req_size) {
                // 这是一个不完整的请求
                m_error = HttpErrorIdx::NoRequest;
                return LineRet::LINE_OPEN;
            } else if (m_req[m_parse_idx + 1] == '\n') {
                m_req[m_parse_idx++] = '\0';
                m_req[m_parse_idx++] = '\0';
                // 这是一个完整行
                return LineRet::LINE_OK;
            }
            // 否则就是一个错误的请求
            m_error = HttpErrorIdx::RequestBad;
            return LineRet::LINE_BAD;
        } else if (tmp == '\n') {
            if (m_parse_idx > 1 && m_req[m_parse_idx-1] == '\r') {
                m_req[m_parse_idx-1] = '\0';
                m_req[m_parse_idx++] = '\0';
                return LineRet::LINE_OK;
            }
            
            // 否则是一个错误的请求
            m_error = HttpErrorIdx::RequestBad;
            return LineRet::LINE_BAD;
        }
    }

    // 否则是一个不完整的行
    m_error = HttpErrorIdx::NoRequest;
    return LineRet::LINE_OPEN;
}

// 开始解析
int HttpRequest::start() {
    // 调用解析主进程
    if (m_req_size <= 0) {
        return HttpErrorIdx::RequestReadDataFailed; // 读取请求数据失败
    }
    return parseMain();
}



/**
 * @brief Construct a new Http Response:: Http Response object
 * 
 * @param req 
 */

HttpResponse::HttpResponse(int capacity) {
    m_response_capacity = capacity;
    m_response_data = (char *)malloc(sizeof(char) * m_response_capacity);
    m_response_size = 0;
}

HttpResponse::~HttpResponse() {
    free(m_response_data);
}

// 添加响应行
bool HttpResponse::addResponseHeader(const char *version, int http_code, const char *state_describe) {
    int size = 0;
    size = sprintf(m_response_data+m_response_size, "%s %d %s\r\n", version, http_code, state_describe);
    if (size < 0) 
        return false;
    m_response_size += size;
    return true;
}

// 添加响应头字段
bool HttpResponse::addHeaderField(const char *key, const char *value) {
    int size = 0;
    size = sprintf(m_response_data+m_response_size, "%s: %s\r\n", key, value);
    if (size < 0)
        return false;
    m_response_size += size;
    return true;
}

bool HttpResponse::addHeaderField(const char *key, int value) {
    int size = 0;
    size = sprintf(m_response_data+m_response_size, "%s: %d\r\n", key, value);
    if (size < 0) 
        return false;
    m_response_size += size;
    return true;
}

// 添加响应体
bool HttpResponse::addHeaderContent(const char *content) {
    // 先添加 \r\n
    int size = 0;
    size = sprintf(m_response_data+m_response_size, "\r\n");
    if (size < 0)
        return false;
    m_response_size += size;
    
    // 先判断当前的容量是否能容纳当前的内容
    int content_size = strlen(content);
    printf("content_size = %d\n", content_size);
    // 如果一直大于，就需要一直扩容
    while ((m_response_size + content_size) >= m_response_capacity) {
        if (!dilatation()) {
            return false;
        }
    }

    // 拷贝内容
    size = sprintf(m_response_data+m_response_size, "%s", content);
    printf("size = %d\n", size);
    printf("response: %s\n", m_response_data);
    if (size < 0) 
        return false;
    m_response_size += size;
    printf("response_size: %ld\n", m_response_size);
    return true;
}

bool HttpResponse::addHeaderContent(const string &content) {
    // 先添加 \r\n
    int size = 0;
    size = sprintf(m_response_data+m_response_size, "\r\n");
    if (size < 0)
        return false;
    
    // 当前空间是否能容纳当前内容，若不能这需要扩容
    while ((m_response_size + content.size()) >= m_response_capacity) {
        if (!dilatation()) {
            return false;
        }
    }

    // 拷贝内容
    size = sprintf(m_response_data+m_response_size, "%s", content.c_str());
    if (size < 0)
        return false;
    m_response_size += size;
    return true;
}

// 扩容函数
bool HttpResponse::dilatation() {
    // 保存久的大小
    int old_capacity = m_response_capacity;
    m_response_capacity *= 2;
    // 保存久的数据地址
    char *old_data = m_response_data;
    // 扩容
    m_response_data = (char *)realloc(m_response_data, m_response_capacity);
    if (m_response_data == nullptr) {
        // 表示扩容失败
        m_response_capacity = old_capacity;
        m_response_data = old_data;
        return false;
    }

    return true;
}

// 传入socket套接字，在内部读取数据
Http::Http(int sockfd, int burst_mode) {
    m_request = new HttpRequest(sockfd, burst_mode);
    m_response = new HttpResponse();
    m_sockfd = sockfd;
}

// 传入socket套接字及从套接字中读取的数据
Http::Http(int sockfd, const char *req_data) { 
    m_request = new HttpRequest(req_data);
    m_response = new HttpResponse();
    m_sockfd = sockfd;
}

Http::~Http() {
    delete m_request;
    delete m_response;
}

// 发送response
int Http::sendResponse() {
    printf("response: \n%s\n", m_response->getResponseData());
    return send(m_sockfd, m_response->getResponseData(), m_response->getResponseDataSize(), 0);
}