#ifndef __M_MACRO_HPP__
#define __M_MACRO_HPP__

/**
 * @file mmacro.hpp
 * @author aloneisbestes@gmail.com
 * @brief 程序中一些相关宏的定义
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// 定义使用的模式，epoll使用的模式，是EPOLLIN还是EPOLLET，该模式可以组合使用，例如: MODTH_IN | MODTH_ET
#define EPOLLIN_MODE    0x01
#define EPOLLET_MODE    0x02

// 定义一行的最长数据
#define MAX_LINE        8192

#endif // __M_MACRO_HPP__