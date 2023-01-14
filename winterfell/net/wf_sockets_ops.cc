/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 21:25:26
 * @Description: 一系列操作socket的封装
 */

#include "winterfell/net/wf_socket.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_sockets_ops.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

namespace winterfell
{
int sockets::createNonblocking() {
  /**
   * AF_INET: IPv4 Internet protocols
   * SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams.  An out-of-band data transmission mechanism may be supported.
   * SOCK_NONBLOCK: 设置为非阻塞，当无法读or写时，errno=EAGAIN
   * SOCK_CLOEXEC：close-on-exec
   */
  int sockfd = ::socket(AF_INET,
                        SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                        IPPROTO_TCP);
  if (sockfd < 0)
  {
    LOG_FATAL << "sockets::createNonblocking";
  }
  return sockfd;
}
void sockets::bind(int sockfd, const struct sockaddr_in* addr) {
  int ret = ::bind(sockfd, reinterpret_cast<const struct sockaddr*> (addr), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
  if (ret < 0) {
    LOG_FATAL << "sockets::bind";
  }
}
void sockets::listen(int sockfd, int backlog) {
  int ret = ::listen(sockfd, backlog);
  if (ret < 0) {
    LOG_FATAL << "sockets::listen";
  }
}

int sockets::accept(int sockfd, struct sockaddr_in *addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
  int connfd = ::accept4(sockfd, reinterpret_cast<struct sockaddr *>(addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0) {
    LOG_ERROR << "sockets::accept";
    int savedErrno = errno;
    switch (savedErrno)
    {
    case EAGAIN:
      // 重试
      break;
    
    default:
      break;
    }
  }
  return connfd;
}

void sockets::shutdownWrite(int sockfd)
{
  if (::shutdown(sockfd, SHUT_WR) < 0)
  {
    LOG_ERROR << "sockets::shutdownWrite";
  }
}

void sockets::setReuseAddr(int sockfd, bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
}

void sockets::close(int fd) {
  ::close(fd);
}

}
