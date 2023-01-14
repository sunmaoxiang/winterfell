/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 15:00:59
 * @Description: 定义TcpConnection class，当消息传来后，会调用指定connection的回调函数
 */

#include "winterfell/net/wf_tcp_connection.h"
#include "winterfell/net/wf_socket.h"
#include <unistd.h>
#include <utility>


namespace winterfell {
TcpConnection::TcpConnection(EventLoop* loop, std::string connName, Socket&& sock , const Endpoint& localEndpoint, const Endpoint& peerEndpointArg) 
: loop_(loop),
  name_(connName),
  socket_(std::unique_ptr<Socket>(new Socket( std::forward<Socket>(sock) ) )), 
  channel_(std::unique_ptr<Channel>(new Channel(loop, socket_->fd()))),
  localEndpoint_(localEndpoint),
  peerEndpoint_(peerEndpointArg) {
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->enableReading();
}
void TcpConnection::handleRead() {
  bufsize = ::read(channel_->fd(), buf, sizeof buf);
  messageCallback_(shared_from_this());
}
}

