/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 15:00:59
 * @Description: 定义TcpConnection class，当消息传来后，会调用指定connection的回调函数
 */

#include "winterfell/net/wf_tcp_connection.h"
#include "winterfell/net/wf_socket.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_buffer.h"
#include <unistd.h>
#include <utility>

using namespace std::placeholders;

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
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    loop_->runInLoop(std::bind(&TcpConnection::enableReadInLoop, this));
}
void TcpConnection::enableReadInLoop() {
  channel_->enableReading();
}
TcpConnection::~TcpConnection() {
  assert(!connected());
}
void TcpConnection::handleRead() {
  int savedErrno;
  auto bufsize = inputBuffer_.readFd(channel_->fd(), &savedErrno);
  if (bufsize > 0) {
    messageCallback_(shared_from_this(), inputBuffer_);
  } else if (bufsize == 0) {
    handleClose();
  } else {
    handleError();
  }
}

void TcpConnection::handleClose() {
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::handleClose state = " << state_;
  assert(state_ == kConnected || state_ == kDisconnecting);
  // we don't close fd, leave it to dtor, so we can find leaks easily.
  channel_->disableAll();
  closeCallback_(shared_from_this());
}

void TcpConnection::handleError() {
  LOG_ERROR << "TcpConnection::hanleError [" << name_ 
            << "] - SO_ERROR = ";
}

void TcpConnection::handleWrite() {
  loop_->assertInLoopThread();
  if (channel_->isWriting()) {
    ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
    if (n > 0) {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0) {
        channel_->disableWriting();
        if (writeCompleteCallback_) {
          loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
        }
        if (state_ == kDisconnecting) {
          shutdownInLoop();
        }
      } else {
        LOG_TRACE << "I am going to write more data";
      }
    } else {
      LOG_ERROR << "TcpConnection::handleWrite";
    }
  } else {
    LOG_TRACE << "Connection is down, no more writing";
  }
}

void TcpConnection::connectDestroyed()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::connectDestroyed : connection between " << localEndpoint_.getIpPort() << " and " << peerEndpoint_.getIpPort() << " is removed"; 
  assert(state_ == kConnected || state_ == kDisconnecting);
  setState(kDisconnected);
  channel_->disableAll();
  loop_->removeChannel(channel_.get());
}

void TcpConnection::connectEstablished() {
  loop_->assertInLoopThread();
  setState(kConnected); // kDisConnected -> kConnected
  channel_->enableReading();
  connectionCallback_(shared_from_this());
}

void TcpConnection::send(const std::string &message) {
  if (state_ == kConnected) {
    if (loop_->isInLoopThread()) {
      sendInLoop(message);
    } else {
      loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
    }
  }
  
}

// 主动断开链接，发送fin，后变为FIN_WAIT1，然后收到ack后变为FIN_WAIT2，对端变为ClOSE_WAIT
void TcpConnection::shutdown() {
  if (state_ == kConnected) {
    setState(kDisconnecting);
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
  }
}
void TcpConnection::sendInLoop(const std::string &message) {
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
    nwrote = ::write(channel_->fd(), message.data(), message.size());
    if (nwrote >= 0) {
      if (static_cast<size_t>(nwrote) < message.size()) {
        LOG_TRACE << "I am going to write more data";
        outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
        if(!channel_->isWriting()) {
          channel_->enableReading();
        }
      } else if (writeCompleteCallback_){
        loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
      }
    } else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        LOG_ERROR << "TcpConnection::sendInLoop";
      }
    }
  }
}
void TcpConnection::shutdownInLoop() {
  loop_->assertInLoopThread();
  if (!channel_->isWriting()) {
    socket_->shutdownWrite();
  }
}


}

