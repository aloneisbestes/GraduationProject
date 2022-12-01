#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <malloc.h>
#include <string>
#include <strings.h>
#include <stdlib.h>
#include "HttpRequest.hpp"
#include "common.hpp"
#include "ErrorMessage.hpp"

using std::string;

HttpRequest::HttpRequest(int sockfd, int mode) 
    : HttpBase(sockfd), m_mode(mode), m_size(0)
{
    m_capacity = DefaultCapacity;
    m_data = new char[m_capacity];
    m_start = 0;
    m_now_idx = 0;
    m_parse_state = ParseStatus::ParseLine; // 初始化为请求请求行
}

HttpRequest::HttpRequest(const HttpRequest &req)
    : HttpBase(req)
{
    m_capacity = req.m_capacity;
    m_data = (char *)malloc(sizeof(char) * m_capacity);
    memset(m_data, 0, m_capacity);
    std::strcpy(m_data, req.m_data);
    m_header = req.m_header;
    m_start = req.m_start;
    m_version = req.m_version;
    m_size = req.m_size;
    m_mode = req.m_mode;
    m_now_idx = req.m_now_idx;
    m_parse_state = req.m_parse_state;
}

HttpRequest::~HttpRequest() {
    if (m_data != nullptr)
        delete m_data;
}

int HttpRequest::run() {
    int ret;
    // 读取数据
    if ((ret = readData()) != Success) {
        // 表示读取数据失败
        return ret;
    }

    // 主要的解析逻辑
    ParseStatus parse_ret;
    LineStatus line_state = LineStatus::LineComplete;
    m_parse_state = ParseStatus::ParseLine;
    char *text = nullptr;
    while ((line_state == LineComplete && m_parse_state == ParseContent) || \
           (line_state = getLine()) == LineComplete) 
    {
        text = m_data + m_start;
        m_start = m_now_idx;
        switch (m_parse_state) {
            case ParseStatus::ParseLine: {
                // 解析请求行
                parse_ret = parseRequestLine(text);
                // 对返回值做处理
                break;
            }
            case ParseStatus::ParseHeader: {
                // 解析请求头
                parse_ret = parseRequestHeader(text);
                // 对返回值做处理
                break;
            }
            case ParseStatus::ParseContent: {
                parse_ret = parseRequestContent(text);
                // 对返回值做处理
                break;
            }
            default: {
                // 如果是其它请求，则表示服务器错误，设置 erridx，并直接return
                //erridx = 
                // return 
            }
        }
    }

    return Success;
}

// 读取socket网络套接字上的数据
int HttpRequest::readData() {
    char buffer[MAX_LINE]={0};
    int byte=0;
    if (m_mode == -1 || m_mode & EPOLLIN_MODE) {
        // 直接读取数据即可, 使用recv读取数据
        byte = recv(m_sockfd, buffer, MAX_LINE, 0); // 读取数据
        if (byte < 0) {
            // 如果读取到的字节数小于0，表示读取失败
            erridx = ErrorIndex::ReadDataFailed;
            return erridx;
        }

        // 拷贝数据到 m_data
        if (byte >= m_capacity) {
            if (!dilatation()) {
                // 表示扩容失败
                erridx = ErrorIndex::ExpansionFailure;
                return erridx;
            }
        } 
        std::strncpy(m_data, buffer, byte);
        m_size = byte;
    } else if (m_mode & EPOLLET_MODE) {
        // 使用epoll机制的EPOLLET，为非阻塞读取，所以需要使用循环一次性读取完
        while (1) {
            // 循环读取非阻塞套接字上的书籍
            byte = recv(m_sockfd, buffer, MAX_LINE, 0);
            if (byte < 0) {
                if (EINTR == errno || EWOULDBLOCK == errno || EAGAIN == errno) {
                    byte = 0;
                } else {
                    erridx = ErrorIndex::ReadDataFailed;    // 读取数据失败
                    return erridx;
                }
            } else if (byte == 0) {
                // 表示读取完成
                break;
            } else {
                // 表示读取到数据，需要拷贝下来
                if ((m_size + byte) >= m_capacity) {
                    if (!dilatation()) {
                        erridx = ErrorIndex::ExpansionFailure;  // 扩容失败
                        return erridx;
                    }
                }
                std::strncpy(m_data+m_size, buffer, byte);
                m_size += byte;
            }
        }
    } else {
        // 匹配错误，服务端内部错误
        erridx = ErrorIndex::WrongChoices;
        return erridx;
    }
    return Success;
}   

// 解析请求行
HttpRequest::ParseStatus HttpRequest::parseRequestLine(char *line) {
    char *tmp = nullptr;

    // 获取请求类型
    // line = strpbrk(tmp, " ");
    line += strspn(tmp, " \t");
    tmp = strchr(line, ' ');
    if (tmp == nullptr)
        return ParseStatus::ParseFault;
    *tmp = '\0';
    if (strncasecmp(line, "GET", 4) == 0) { // GET 请求
        m_type = RequestType::GetRequest;
    } else if (strncasecmp(line, "POST", 4) == 0) { // POST 请求
        m_type = RequestType::PostRequest;
    } else {
        // 表示这不是一个能处理的请求
        return ParseStatus::ParseFault;
    }

    // 获取url
    line += strspn(tmp, " \t");
    tmp = strchr(line, ' ');
    if (tmp == nullptr) 
        return ParseStatus::ParseFault;
    *tmp = '\0';
    m_url = line;

    // 获取http协议版本号
    tmp = strchr(line, ' ');
    if (tmp == nullptr)    
        return ParseStatus::ParseFault;    
    *tmp = '\0';
    m_version = line;

    m_parse_state = ParseStatus::ParseHeader;
    return ParseStatus::ParseComplete;
}

// 解析请求体
HttpRequest::ParseStatus HttpRequest::parseRequestHeader(char *line) {

}

// 解析请求携带的内容
HttpRequest::ParseStatus HttpRequest::parseRequestContent(char *line) {

}

// 扩容操作
bool HttpRequest::dilatation() {
    // 保存之前开辟的内存地址
    char *old_data = m_data;
    if ((m_data = (char *)realloc(m_data, m_capacity * 2)) == NULL) {
        // 如果开辟失败，表示内存不足，返回false
        m_data = old_data;
        return false;
    }

    // 开辟成功，容量扩容，返回true
    m_capacity *= 2;
    return true;
}

// 获取请求数据的一行内容
HttpRequest::LineStatus HttpRequest::getLine() {
    for (m_now_idx; m_now_idx < m_size; m_now_idx++) {
        if (m_data[m_now_idx] == '\r') {
            if ((m_now_idx+1) >= m_size) {
                // 表示当前是一个不完整的请求
                return LineStatus::LineIncomplete;  // 不完整的请求
            }
            if (m_data[m_now_idx+1] == '\n') {
                // 当前一行是完整的
                // 表示当前所处 \r 位置, \r\n
                m_data[m_now_idx++] = '\0';
                m_data[m_now_idx++] = '\0'; // 这里是将\r\n替换为\0
                return LineStatus::LineComplete;    // 解析一行完成
            }
            return LineStatus::LineFault;   // 错误一行，表示错误的请求
        } else if (m_data[m_now_idx] == '\n') {
            if (m_now_idx >= 1 && m_data[m_now_idx-1] == '\r') {
                // 表示当前所处位置是 \n, \r\n
                m_data[m_now_idx-1] = '\0';
                m_data[m_now_idx++] = '\0';
                return LineStatus::LineComplete;
            }
            return LineStatus::LineFault;   // 错误一行，表示当前的请求是错误
        }
    }

    return LineStatus::LineIncomplete;  // 不完整的请求
}