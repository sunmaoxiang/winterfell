/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 16:20:44
 * @Description: 请填写简介
 */

#include "winterfell/net/wf_tcp_server.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_endpoint.h"
#include "winterfell/net/wf_tcp_connection.h"
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
using namespace std;
using namespace winterfell;
void onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    LOG_INFO << "onConnection: new connection [ " << conn->name() << "] from " << conn->peerEndpoint().getIpPort() ;
  } else {
    LOG_INFO << "onConnection: connection [ " << conn->name() << "] is down" ;
  }
}
void onMessage(const TcpConnectionPtr& conn) {
  cout << "recv msg: " << conn->getBuf();
  cout << "recv size: " << conn->getBufSize() << endl;;
}


int main() {
  LOG_INFO << "main(): pid = %d\n" << ::getpid();
  Endpoint listenEndpoint("192.168.158.12", 9981);
  EventLoop loop;

  TcpServer server(&loop, listenEndpoint);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();
  loop.loop();
  return 0;
}