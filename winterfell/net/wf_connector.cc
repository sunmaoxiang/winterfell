/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-05 16:42:19
 * @Description: Connector类定义，主要处理Client连接建立时回调
 */

#include "winterfell/net/wf_connector.h"
#include "winterfell/net/wf_channel.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_sockets_ops.h"

#include <errno.h>



namespace winterfell {

const int Connector::kInitRetryDelayMs;
const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const Endpoint& serverEndPoint)
: loop_(loop),
  serverEndPoint_(serverEndPoint),
  connect_(false),
  state_(kDisconnected),
  retryDelayMs_(kInitRetryDelayMs) {
  }

Connector::~Connector() {
  assert(!channel_);
}

void Connector::start() {
  connect_ = true;
  loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop() {
  loop_->assertInLoopThread();
  assert(state_ == kDisconnected);
  if (connect_) {
    connect();
  } else {
    LOG_ERROR << "do not connect";
  }
}

void Connector::stop() {
  connect_ = false;
  loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop() {
  loop_->assertInLoopThread();
  if(state_ == kConnecting) {
    setState(kDisconnected);
    int sockfd = removeAndResetChannel();
    retry(sockfd);
  }
}

void Connector::connect() {
  int sockfd = sockets::createNonblocking();
  int ret = sockets::connect(sockfd, serverEndPoint_.getSockAddr());
  int savedErrno = (ret == 0) ? 0 : errno;
  switch (savedErrno) {
  case 0:
  case EINPROGRESS:
  case EINTR:
  // 套接字已经连接
  case EISCONN:
    connecting(sockfd);
    break;
  // Network is unreachable.
  case EAGAIN:
  case EADDRINUSE:
  case EADDRNOTAVAIL:
  case ECONNREFUSED:
  // Network is unreachable.
  case ENETUNREACH:
    retry(sockfd);
    break;
  default:
    LOG_ERROR << "error in Connector::startInLoop " << savedErrno;
    sockets::close(sockfd);
    break;
  }
}

void Connector::restart() {
  loop_->assertInLoopThread();
  setState(kDisconnected);
  retryDelayMs_ = kInitRetryDelayMs;
  connect_ = true;
  startInLoop();
}

void Connector::connecting(int sockfd) {
  setState(kConnecting);
  assert(!channel_);
  channel_.reset(new Channel(loop_, sockfd));
  channel_->setWriteCallback(std::bind(&Connector::handlewrite, this));
  channel_->setErrorCallback(std::bind(&Connector::handleError, this));
  channel_->enableWriting();
}

int Connector::removeAndResetChannel() {
  channel_->disableAll();
  channel_->remove();
  int sockfd = channel_->fd();
  loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
  return sockfd;
}

void Connector::resetChannel() {
  channel_.reset();
}

void Connector::handlewrite() {
  LOG_TRACE << "Connector::hanleWrite " << state_;

  if (state_ == kConnecting) {
    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    if (err) {
      LOG_WARN << "Connector::handleWrite - SO_ERROR = " << err;
      retry(sockfd);
    } else {
      setState(kConnected);
      if (connect_) {
        newConnectionCallback_(sockfd);
      } else {
        sockets::close(sockfd);
      }
    }
  } else {
    assert(state_ == kDisconnected);
  }
}

void Connector::handleError() {
  LOG_ERROR << "Connector::handleError state=" << state_;
  if (state_ == kConnecting) {
    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    LOG_TRACE << "SO_ERROR = " << err ;
    retry(sockfd);
  }
}

void Connector::retry(int sockfd) {
  sockets::close(sockfd);
  setState(kDisconnected);
  if (connect_) {
    LOG_INFO << "Connector::retry - Retry connecting to " << serverEndPoint_.getIpPort() 
             << " in " << retryDelayMs_ << " milliseconds. " ;

    loop_->runAfter(retryDelayMs_ / 1000.0, std::bind(&Connector::startInLoop, shared_from_this()));
    retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
  } else {
    LOG_DEBUG << "do not connect" ;
  }
}


}