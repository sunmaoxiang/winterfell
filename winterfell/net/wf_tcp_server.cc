/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 14:00:56
 * @Description: 定义TcpServer类 管理Acceptor获得的TcpConnection
 */

#include "winterfell/net/wf_tcp_server.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_endpoint.h"
#include "winterfell/net/wf_sockets_ops.h"
#include "winterfell/net/wf_acceptor.h"
#include "winterfell/net/wf_tcp_connection.h"
using namespace std::placeholders;

namespace winterfell {

TcpServer::TcpServer(EventLoop* loop, const Endpoint& listenEndpoint, std::string name)
:name_(name),
loop_(loop),
acceptor_( std::unique_ptr<Acceptor>(new Acceptor(loop,listenEndpoint))),
nextConnId_(1) {  
  acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnectionCallback, this, _1, _2) );
}

void TcpServer::newConnectionCallback(Socket sock, const Endpoint& peerEndpoint) {
  loop_->assertInLoopThread();
  char buf[32];
  snprintf(buf, sizeof buf, "#%d", nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf; // TcpServer的名字 + # + connId
  LOG_INFO << "TcpServer::newConnection [" << name_ << "] - new connection [" 
           << connName << "] from " << peerEndpoint.getIpPort();
  Endpoint localEndpoint(sockets::getLocalAddr(sock.fd()));
  
  // 将新连接储存起来
  TcpConnectionPtr conn(new TcpConnection(loop_, connName, std::move(sock), localEndpoint, peerEndpoint));
  connections_[connName] = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
  conn->setWriteCompleteCallback(writeCompleteCallback_); 
  conn->connectEstablished();
}
void TcpServer::start() {
  if(!started_) {
    started_ = true;
  } 
  if (!acceptor_->listening()) {
    loop_->runInLoop(std::bind(&Acceptor::listen, this->acceptor_.get()));
  }
}

TcpServer::~TcpServer() {
  connections_.clear();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
  loop_->assertInLoopThread();
  LOG_INFO << "TcpServer::removeConnection [" << name_ 
           << "] - connection " << conn->name();
  size_t n = connections_.erase(conn->name());
  assert(n == 1);
  loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn)); 
}
}