/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-05 16:41:40
 * @Description: TcpClient类声明
 */
#pragma once
#include "winterfell/base/wf_mutex.h"
#include "winterfell/net/wf_tcp_connection.h"
namespace winterfell {

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : Noncopyable {
public:
  TcpClient(EventLoop* loop, const Endpoint& endPoint, const string& nameArg);
  ~TcpClient();
  void connect();
  void disconnect();
  void stop();

  TcpConnectionPtr connection() const {
    MutexLockGuard lock(mutex_);
    return connection_;
  }
  EventLoop* getLoop() const {return loop_;}
  bool retry() const { return retry_; }
  void enableRetry() { retry_ = true; }

  const string& name() const {
    return name_;
  }

  void setConnectionCallback(ConnectionCallback cb) {
    connectionCallback_ = std::move(cb);
  }
  void setMessageCallback(MessageCallback cb) {
    messageCallback_ = std::move(cb);
  }
  void setWriteCompleteCallback(WriteCompleteCallback cb) {
    writeCompleteCallback_ = std::move(cb);
  }
  void send() {}
private:
  void newConnection(int sockfd);
  void removeConnection(const TcpConnectionPtr& conn);

  EventLoop* loop_;
  ConnectorPtr connector_;
  const string name_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;

  bool retry_;
  bool connect_;
  int nextConnId_;
  mutable MutexLock mutex_;
  TcpConnectionPtr connection_;

};
}