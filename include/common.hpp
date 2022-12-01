#ifndef __COMMON_HPP__
#define __COMMON_HPP__

/**
 * @file common.hpp
 * @author aloneisbestes@gmail.com
 * @brief 
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>

/** 打印测试宏 **/
#define DebugPrint(format, ...) { \
    printf("Debug: ");\
    printf(format, __VA_ARGS__);\
}

#endif // __COMMON_HPP__