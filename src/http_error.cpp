#include "http_error.h"

int http_error_code=1;      // http的错误码

const char http_error[][HTTP_STRING_CODE_MAX] = {
    "",
    "This is a wrong request",              // 这是一个错误的请求
    "This is an incomplete request",        // 这是一个不完整的请求
    "The get request parameter is incorrect", // get请求参数错误
    "Failed to read the requested data. Procedure",     // 读取请求数据失败
};

const char *httpErrorCode(int http_code) {
    return http_error[http_code];
}