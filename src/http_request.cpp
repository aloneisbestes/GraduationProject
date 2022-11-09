#include <cstring>
#include "HttpRequest.hpp"
std::string;

HttpRequest::HttpRequest(int sockfd=-1) 
    : HttpBase(sockfd)
{
    m_capacity = DefaultCapacity;
    m_data = new char[m_capacity];
    m_start = 0;
}

HttpRequest::HttpRequest(const HttpRequest &req)
    : HttpBase(req)
{
    m_capacity = req.m_capacity;
    m_data = new char[m_capacity];
    std::strcpy(m_data, req.m_data);
    m_header = req.m_header;
    m_start = req.m_start;
    m_version = req.m_version;
}

HttpRequest::~HttpRequest() {
    if (m_data != nullptr)
        delete m_data;
}

bool HttpRequest::run() {
    // 主要的解析逻辑
}