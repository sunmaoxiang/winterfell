/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 15:00:59
 * @Description: 定义TcpConnection class，当消息传来后，会调用指定connection的回调函数
 */

#include "winterfell/net/wf_tcp_connection.h"
#include "winterfell/net/wf_socket.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include <unistd.h>
#include <utility>


namespace winterfell {
TcpConnection::TcpConnection(EventLoop* loop, std::string connName, Socket&& sock , const Endpoint& localEndpoint, const Endpoint& peerEndpointArg) 
: loop_(loop),
  name_(connName),
  state_(TcpConnection::kConnnecting),
  socket_(std::unique_ptr<Socket>(new Socket( std::forward<Socket>(sock) ) )), 
  channel_(std::unique_ptr<Channel>(new Channel(loop, socket_->fd()))),
  localEndpoint_(localEndpoint),
  peerEndpoint_(peerEndpointArg) {
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    channel_->enableReading();
}
TcpConnection::~TcpConnection() {
  assert(!connected());
}
void TcpConnection::handleRead() {
  bufsize = ::read(channel_->fd(), buf, sizeof buf);
  if (bufsize > 0) {
    messageCallback_(shared_from_this());
  } else if (bufsize == 0) {
    handleClose();
  } else {
    handleError();
  }
}

void TcpConnection::handleClose() {
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::handleClose state = " << state_;
  assert(state_ == kConnected);
  // we don't close fd, leave it to dtor, so we can find leaks easily.
  channel_->disableAll();
  closeCallback_(shared_from_this());
}

void TcpConnection::handleError() {
  LOG_ERROR << "TcpConnection::hanleError [" << name_ 
            << "] - SO_ERROR = ";
}

void TcpConnection::connectDestroyed() {
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::connectDestroyed : connection between " << localEndpoint_.getIpPort() << " and " << peerEndpoint_.getIpPort() << " is removed"; 
  assert(state_ == kConnected);
  setState(kDisconnected);
  channel_->disableAll();
  loop_->removeChannel(channel_.get());
}

void TcpConnection::connectEstablished() {
  loop_->assertInLoopThread();
  setState(kConnected);
  channel_->enableReading();
  connectionCallback_(shared_from_this());
}

}

