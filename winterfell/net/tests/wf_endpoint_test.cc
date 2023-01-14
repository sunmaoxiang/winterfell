/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-13 19:44:18
 * @Description: 测试endpoint类
 */

#include <iostream>
#include <typeinfo>
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_endpoint.h"
using namespace std;
using namespace winterfell;
int main() {
  Endpoint e0("10.0.0.1", 1234);
  setLoggerLevel(Logger::INFO);
  LOG_INFO << "IP:" << e0.getIp();
  LOG_INFO << "PORT:"<< e0.getIpPort();  

  Endpoint e1(1234);
  LOG_INFO << "IP:" << e1.getIp();
  LOG_INFO << "PORT:" << e1.getIpPort();

  //  ./bin/wf_endpoint_test  | c++filt
  Endpoint e3();
  LOG_INFO << "e3 type: " << typeid(e3).name(); // winterfell::Endpoint ()
  

  Endpoint e4(0);
  LOG_INFO << "e4 type: " << typeid(e4).name(); // winterfell::Endpoint
}
