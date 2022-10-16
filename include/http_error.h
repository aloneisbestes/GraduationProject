#ifndef __HTTP_ERROR_H__
#define __HTTP_ERROR_H__

/**
 * @brief 
 * http 的错误码
 */

// 定义错误码对应的字符长度
#define HTTP_STRING_CODE_MAX    1024

// 定义错误对应的下标
enum HttpErrorIdx {
    RequestBad=1,           // 错误/坏的请求
    NoRequest,              // 不完整的请求
    GetRequestArgsError,    // get请求参数错误
    RequestReadDataFailed,  // 读取请求数据失败
};

// 外部链接错误码字符
extern const char http_error[][HTTP_STRING_CODE_MAX];
// 外部链接错误码
extern int http_error_code;

// 返回http错误结果字符
const char *httpErrorCode(int http_code);

#endif //__HTTP_ERROR_H__