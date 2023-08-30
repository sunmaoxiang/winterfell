/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 19:24:56
 * @Description: 使用RAII管理socket，并封装操作sockfd的一系列操作
 */


#include "winterfell/net/wf_acceptor.h"
#include "winterfell/net/wf_sockets_ops.h"
#include "winterfell/net/wf_socket.h"
#include "winterfell/net/wf_endpoint.h"

#include <unistd.h>
namespace winterfell {
Socket::~Socket() {
  if (sockfd_ != nullptr) {
    ::close(fd());
    delete sockfd_;
  }
}

void Socket::bindAddress(const Endpoint& endpoint) {
  sockets::bind(fd(), endpoint.getSockAddr());
}
void Socket::listen() {
  sockets::listen(fd()); 
}
int  Socket::accept(Endpoint* peerEndpoint) {
  sockaddr_in sockAddr;
  int connfd = sockets::accept(fd(), &sockAddr); 
  peerEndpoint->setSockAddr(sockAddr);
  return connfd;
}
void Socket::shutdownWrite() {
  sockets::shutdownWrite(fd());
}

void Socket::setReuseAddr(bool on) {
  sockets::setReuseAddr(fd(), on);
}

void Socket::setTcpNoDelay(bool on) {

}
}