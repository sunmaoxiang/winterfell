/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 19:25:14
 * @Description: typdedef所有callbacks
 */

#pragma once

#include <functional>

namespace winterfell {

class Endpoint;
class Socket;
/**
 * @brief: 用于注册到Channel
 */  
typedef std::function<void()> EventCallback;

/**
 * @brief 用于注册runAt, runAfter, runEvery
*/
typedef std::function<void()> TimerCallback;

/**
 * @brief 用于回调新的Connection连接
*/
typedef std::function<void (Socket sock, const Endpoint&)> NewConnectionCallback;

}