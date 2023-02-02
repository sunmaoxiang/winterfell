/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-05 16:41:51
 * @Description: TcpClient类定义
 */
#include "winterfell/net/wf_tcp_client.h"
#include "winterfell/net/wf_connector.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_sockets_ops.h"
#include "winterfell/net/wf_socket.h"

namespace winterfell {

namespace detail {

void removeConnection(EventLoop *loop, const TcpConnectionPtr &conn) {
  loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}
}

using namespace std::placeholders;
TcpClient::TcpClient(EventLoop* loop, const Endpoint& endPoint, const string& nameArg)
: loop_(loop), 
  connector_(new Connector(loop, endPoint)),
  retry_(false),
  connect_(true), 
  nextConnId_(1) {
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, _1));
    LOG_INFO << "TcpClient::TcpClient[ " << this
             << "] - connector " << connector_.get();
}
TcpClient::~TcpClient() {
  LOG_INFO << "TcpClient::~TcpClient[ " << this
             << "] - connector " << connector_.get();
  TcpConnectionPtr conn; // COW copy-on-write
  {
    MutexLockGuard lock(mutex_);
    conn = connection_;
  }
  if (conn) {
    CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
    loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
  } else {
    connector_->stop();
  }

}
void TcpClient:: connect() {
  LOG_INFO << "TcpClient::connect[" << this << "] - connecting to " 
           << connector_->serverEndPoint().getIpPort();
  connect_ = true;
  connector_->start();
}
void TcpClient:: disconnect() {
  connect_ = false;
  {
    MutexLockGuard lock(mutex_);
    if (connection_) {
      connection_->shutdown();
    }
  }
}
void TcpClient:: stop() {
  connect_ = false;
  connector_->stop();
}
void TcpClient::newConnection(int sockfd) {
  loop_->assertInLoopThread();
  Endpoint peer(sockets::getPeerAddr(sockfd));
  char buf[32];
  snprintf(buf, sizeof buf, ":%s#%d", peer.getIpPort().c_str(), nextConnId_);
  ++nextConnId_;
  string connName = buf;
  
  Endpoint localAdder(sockets::getLocalAddr(sockfd));
  Socket sock(sockfd);
  TcpConnectionPtr conn(new TcpConnection(loop_, connName, std::move(sock), localAdder, peer ));
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, _1));
  {
    MutexLockGuard lock(mutex_);
    connection_ = conn;
  }
  conn->connectEstablished();

}
void TcpClient:: removeConnection(const TcpConnectionPtr &conn) {
  loop_->assertInLoopThread();
  {
    MutexLockGuard lock(mutex_);
    assert(connection_ == conn);
    connection_.reset();
  }
  loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
  if (retry_ && connect_) {
    LOG_INFO << "TcpClient::connect[" << this << "] - Reconnecting to "
             << connector_->serverEndPoint().getIpPort(); 
    connector_->restart();
  }

}
}