/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 19:25:14
 * @Description: typdedef所有callbacks
 */

#pragma once

#include "winterfell/base/wf_timestamp.h"

#include <functional>
#include <memory>
namespace winterfell {

class Endpoint;
class Socket;
class Buffer;
/**
 * @brief: 用于注册到Channel
 */  
typedef std::function<void()> EventCallback;

/**
 * @brief 用于注册runAt, runAfter, runEvery
*/
typedef std::function<void()> TimerCallback;

/**
 * @brief Acceptor 用于回调新的Connection连接
*/
typedef std::function<void (Socket sock, const Endpoint&)> NewConnectionCallback;

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
/**
 * @brief TcpConnection 用于有新的连接到来
*/
typedef std::function<void (const TcpConnectionPtr& conn)> ConnectionCallback;

/**
 * @brief 用于有新msg到来后进行回调
*/
typedef std::function<void (const TcpConnectionPtr& conn, Buffer& buf, Timestamp timestamp)> MessageCallback; 

/**
 * @brief 用于client断开连接后进行回调
*/
typedef std::function<void (const TcpConnectionPtr& conn)> CloseCallback;

/**
 * @brief 用于output缓冲区为空时，即写完毕后执行
*/
typedef std::function<void(const TcpConnectionPtr& conn)> WriteCompleteCallback;

/**
 * @brief 用于EventLoop::runInLoop
*/
typedef std::function<void()> Functor;
}