#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <malloc.h>
#include "HttpRequest.hpp"
#include "common.hpp"
#include "ErrorMessage.hpp"
std::string;

HttpRequest::HttpRequest(int sockfd, int mode) 
    : HttpBase(sockfd), m_mode(mode), m_size(0)
{
    m_capacity = DefaultCapacity;
    m_data = new char[m_capacity];
    m_start = 0;
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
}

HttpRequest::~HttpRequest() {
    if (m_data != nullptr)
        delete m_data;
}

bool HttpRequest::run() {
    // 主要的解析逻辑
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
int HttpRequest::parseRequestLine() {

}

// 解析请求体
int HttpRequest::parseRequestHeader() {

}

// 解析请求携带的内容
int HttpRequest::parseRequestContent() {

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
