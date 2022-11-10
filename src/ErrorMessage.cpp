#include "ErrorMessage.hpp"

// 默认初始化为0
int erridx = 0;

// 错误字符数组
char errstr[][ERROR_SIZE_MAX] = {
    "success",
    "Dynamic capacity expansion failed because memory was insufficient.",  // 内存不足，动态扩容失败
    "The wrong match.", // 匹配错误
    "Failed to read data.", // 读取数据失败
    ""
};