/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 18:59:58
 * @Description: 使用RAII管理socket，并封装操作sockfd的一系列操作
 */

#pragma once

#include "winterfell/util/wf_noncopyable.h"
struct tcp_info; // in <netinet/tcp.h>

namespace winterfell {
class Endpoint;
class Socket {
public:
  explicit Socket(int sockfd)
      : sockfd_(new int(sockfd))
  {
  }
  /**
   * @brief: 移动构造函数，转移ownership
   * @param {Socket&&} sock
   * @return {*}
   */  
  Socket(Socket&& sock)
    : Socket(sock.fd()) {
      sock.sockfd_ = nullptr;
    } 
  ~Socket();

  int fd() const {return *sockfd_; }

  bool getTcpInfo(struct tcp_info*) const;
  bool getTcpInfoString(char* buf, int len) const;

  void bindAddress(const Endpoint& endpoint);
  void listen();

  /**
   * @brief: 如果返回一个非负数，代表连接成功，该数是已连接套接字
   *         已经被设置为非阻塞和close-on-exec, peeraddr是output
   *         如果返回-1 则说明出错
   * @param {InetAdress*} peeraddr
   * @return {*}
   */  
  int accept(Endpoint* peerEndpoint);

  void shutdownWrite();
  /**
   * @brief TCP_NODELAY: Nagle's agortithm，当TCP发送缓冲区
   *        数据积累到一定程度一般为MSS时才会发送，防止小包过多导致
   *        网络拥塞，但一般要求低延时的服务关闭此算法
  */
  void setTcpNoDelay(bool on);
  /**
   * @brief SO_REUSEADDR
  */
  void setReuseAddr(bool on);
  /**
   * @brief SO_REUSEPORT
  */
  void setReusePort(bool on);

  /**
   * @brief SO_KEEPALIVE
  */
  void setKeepAlive(bool on);
private:
  const int *sockfd_;
};
}
