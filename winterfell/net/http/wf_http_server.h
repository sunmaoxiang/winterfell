/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-23 15:51:32
 * @Description: HttpServer 声明
 */

#pragma once

#include <winterfell/net/wf_tcp_server.h>
#include <functional>

namespace winterfell {

class HttpRequest;
class HttpResponse;

class HttpServer : Noncopyable {

public:
  typedef std::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;

  HttpServer(EventLoop* loop, const Endpoint& listenAddr, const string& name);

  void setThreadNum(int numThreads) {
    server_.setThreadNum(numThreads);
  }
  void setHttpCallback(const HttpCallback& cb) {
    httpCallback_ = cb;
  }
  void start();

private:
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn, Buffer& buf, Timestamp receiveTime);
  void onRequest(const TcpConnectionPtr&, const HttpRequest&);

  TcpServer server_;
  HttpCallback httpCallback_;
};



}