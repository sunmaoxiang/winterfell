/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-05 16:42:12
 * @Description: Connector类声明，主要处理Client连接建立时回调
 */
#pragma once

#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/net/wf_endpoint.h"

#include <memory>
#include <functional>

namespace winterfell {
class Channel;
class EventLoop;
class Connector : Noncopyable, 
                  public std::enable_shared_from_this<Connector> {

public:
  typedef std::function<void (int sockfd)> NewConnectionCallback;

  Connector(EventLoop* loop, const Endpoint& serverEndPoint);
  ~Connector();

  void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }

  void start();
  void restart();
  void stop();

  const Endpoint& serverEndPoint() const { return serverEndPoint_; }

private:  
  enum States { kDisconnected, kConnecting, kConnected, };
  static const int kMaxRetryDelayMs = 30 * 1000;
  static const int kInitRetryDelayMs = 500;

  void setState(States s) { state_ = s; }
  void startInLoop();
  void stopInLoop();
  void connect();
  void connecting(int sockfd);
  void handlewrite();
  void handleError();
  void retry(int sockfd);
  int removeAndResetChannel();
  void resetChannel();

  EventLoop* loop_;
  Endpoint serverEndPoint_;
  bool connect_;
  States state_;
  std::unique_ptr<Channel> channel_;
  NewConnectionCallback newConnectionCallback_;
  int retryDelayMs_;
};

}