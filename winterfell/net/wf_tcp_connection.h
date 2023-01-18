/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 15:00:43
 * @Description: 声明TcpConnection class，当消息传来后，会调用指定connection的回调函数
 */

#pragma once
#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/net/wf_endpoint.h"
#include "winterfell/net/wf_channel.h"
#include "winterfell/util/wf_callbacks.h"
#include <memory>
#include <string>

namespace winterfell {
class EventLoop;
class Socket;
class TcpConnection : Noncopyable, public std::enable_shared_from_this<TcpConnection> {
public:
  TcpConnection(EventLoop* loop_, std::string connName, Socket&& sock, const Endpoint& localEndpoint, const Endpoint& peerEndpoint);
  ~TcpConnection();
  void setConnectionCallback(ConnectionCallback& cb) { connectionCallback_ = cb; }
  void setMessageCallback(MessageCallback& cb) { messageCallback_ = cb; }
  void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

  void connectEstablished();
  void connectDestroyed();
  bool connected() const { return state_ == kConnected; }

  const string& name() const { return name_; }
  const Endpoint& peerEndpoint() const { return peerEndpoint_;}
  // FIXME: use buf class
  const char* getBuf() const { return buf; }
  int getBufSize() const {return static_cast<int>(bufsize);}
private:
  enum StateE { kConnnecting, kConnected, kDisconnected, }; // 定义状态
  void setState(StateE s) { state_ = s; }
  void handleRead(); // 当有消息到来时调用
  void handleWrite();
  void handleClose();
  void handleError();

  EventLoop* loop_;
  std::string name_;
  StateE state_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  Endpoint localEndpoint_;
  Endpoint peerEndpoint_;
  ConnectionCallback connectionCallback_;  
  MessageCallback messageCallback_;
  CloseCallback closeCallback_;

  // FIXME: use buffer class
  char buf[65536];
  ssize_t bufsize;
};
}