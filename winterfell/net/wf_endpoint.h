/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 19:28:26
 * @Description: Endpoint类声明, 封装struct sockaddr_in
 */
#pragma once

#include <netinet/in.h>
#include <string>
namespace winterfell {

using std::string;

class Endpoint {

public:
  /**
   * @brief 提供port初始化，用于listenfd bind 到本地主机地址
  */
  explicit Endpoint(uint16_t port = 0);
  /**
   * @brief 提供ip + port 初始化
  */
  explicit Endpoint(std::string ip, uint16_t port);
  
  /**
   * @brief 提供 sockaddr_in 初始化
  */
  explicit Endpoint(const struct sockaddr_in& addr) 
    : addr_(addr)
  {}

  /**
   * @brief 一系列getter setter
  */
  sa_family_t family() const { return addr_.sin_family; }
  string getIp() const;
  string getIpPort() const;
  uint16_t getPort() const;
  
  void setSockAddr(const struct sockaddr_in& addr) { addr_ = addr; }
  const struct sockaddr_in* getSockAddr() const { return &addr_; }
  /**
   * hostname -> result
  */
  static bool resolve(string hostname, Endpoint *result);
private:
  struct sockaddr_in addr_;
};

}
