/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 19:35:26
 * @Description: Endpoint类定义，封装struct sockaddr_in
 */

#include "winterfell/net/wf_endpoint.h"

#include "winterfell/base/wf_log.h"

#include <cstring>
#include <endian.h>
#include <arpa/inet.h>

namespace winterfell {
Endpoint::Endpoint(uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  in_addr_t ip = INADDR_ANY;
  addr_.sin_addr.s_addr = htobe32(ip); //ip转网络字节序 （大端）
  addr_.sin_port = htobe16(port); // port转网络字节序 
}

Endpoint::Endpoint(string ip, uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htobe16(port);
  if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr)) {
    LOG_ERROR << "inet_pton error";
  }
}
string   Endpoint::getIp() const {
  return ::inet_ntoa(addr_.sin_addr);
}
string   Endpoint::getIpPort() const {
  return getIp() + ":" + std::to_string(::htons(addr_.sin_port));
}
uint16_t Endpoint::getPort() const {
  return ::htons(addr_.sin_port);
} 
}