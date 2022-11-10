#ifndef __ERROR_MESSAGE_HPP__
#define __ERROR_MESSAGE_HPP__

/**
 * @file ErrorMessage.hpp
 * @author your name (you@domain.com)
 * @brief 错误信息类
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

enum ErrorIndex {
    Success=0,    //第一个是特殊的类别，并不是错误，表示成功
    ExpansionFailure,   // 内存扩容失败
    WrongChoices,   // 错误的选择
    ReadDataFailed, // 读取数据失败
};

#define ERROR_SIZE_MAX  1024

extern int erridx;
extern char errstr[][ERROR_SIZE_MAX];


#endif // __ERROR_MESSAGE_HPP__