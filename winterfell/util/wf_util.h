/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 20:09:56
 * @Description: 声明各种单独的功能函数，例如获取当前线程ID等
 */

#pragma once

#include <pthread.h>
#include <sys/syscall.h>
namespace winterfell {

/**
 * @brief: 获取当前执行流的线程id
 * @return pid_t 当前线程id
 */
pid_t GetThreadId();

}