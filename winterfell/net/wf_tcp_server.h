/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 14:00:47
 * @Description: 声明TcpServer类 管理Acceptor获得的TcpConnection
 */

#pragma once

#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/util/wf_callbacks.h"
#include "winterfell/net/wf_socket.h"
#include <map>
#include <string>

namespace winterfell {

class Endpoint;
class EventLoop;
class Acceptor;

class TcpServer : Noncopyable {

public:
TcpServer(EventLoop* loop, const Endpoint& listenEndpoint, std::string name = "tcp_server");
~TcpServer();

/**
 * @brief 启动一个Tcp服务器
*/
void start();

/**
 * @brief 设置TcpServer创建一个新连接会回调的函数
*/
void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }

/**
 * @brief 设置TcpServer收到一个消息会回调的函数
*/
void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

private:

void newConnectionCallback(Socket sock, const Endpoint& peerEndpoint);


const std::string name_;
EventLoop* loop_;
std::unique_ptr<Acceptor> acceptor_;
ConnectionCallback connectionCallback_;
MessageCallback messageCallback_;
bool started_;

int nextConnId_; // 递增用于标识唯一的Connection

typedef std::map<std::string, TcpConnectionPtr> ConnectionMap; // 保存目前存活的TcpConnection的shared_ptr;
ConnectionMap connections_;
};
}