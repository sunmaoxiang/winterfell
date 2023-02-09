/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-09 15:59:41
 * @Description: 一个Echo服务器，接收到消息原封不动返回
 */

#include "winterfell/net/wf_tcp_server.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_tcp_connection.h"
#include "winterfell/net/wf_event_loop.h"
#include <string>
using namespace winterfell;
using namespace std;
void onConnect(const TcpConnectionPtr &conn) {
  if (conn->connected()) {
    LOG_INFO << "onConnect()" << " " << conn->peerEndpoint().getIpPort() ;
  } else {
    abort();
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer& buf) {
  assert(conn->connected());
  string msg = buf.retrieveAsString();
  conn->send(msg);
}

int main() {
  setLoggerLevel(Logger::INFO);
  EventLoop loop;
  Endpoint listenEndpoint(8888);
  TcpServer s(&loop, listenEndpoint);
  s.setConnectionCallback(onConnect);
  s.setMessageCallback(onMessage);
  s.setThreadNum(4);
  s.start();
  loop.loop();
  return 0;
}