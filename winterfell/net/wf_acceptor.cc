/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 18:44:08
 * @Description: Acceptor class定义，用于accept新TCP连接，并通过回调通知使用者
 */

#include "winterfell/net/wf_acceptor.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_sockets_ops.h"
#include "winterfell/net/wf_endpoint.h"


namespace winterfell {
Acceptor::Acceptor(EventLoop *loop, const Endpoint& listenAddr)
: loop_(loop),
  acceptSocket_(sockets::createNonblocking()),
  acceptChannel_(loop_, acceptSocket_.fd()),
  listenning_(false) {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen() {
  loop_->assertInLoopThread();
  listenning_ = true;
  acceptSocket_.listen();
  acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
  loop_->assertInLoopThread();
  Endpoint peerEndpoint(0);
  int connfd = acceptSocket_.accept(&peerEndpoint);
  Socket sock(connfd);
  if (connfd >= 0) {
    if (newConnectionCallback_) {
      newConnectionCallback_(std::move(sock), peerEndpoint); 
    }
  }
}



}
