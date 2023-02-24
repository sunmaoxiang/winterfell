/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-02 17:08:55
 * @Description: 测试主从reactor
 */

#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_tcp_server.h"
#include "winterfell/net/wf_buffer.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_endpoint.h"
#include "winterfell/net/wf_tcp_connection.h"
#include <sys/types.h>
#include <iostream>
#include <unistd.h>

using namespace winterfell;
using namespace std;
void onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    LOG_INFO << "onConnection: new connection [ " << conn->name() << "] from " << conn->peerEndpoint().getIpPort() ;
  } else {
    LOG_INFO << "onConnection: connection [ " << conn->name() << "] is down" ;
  }
}
void onMessage(const TcpConnectionPtr& conn, Buffer &buf, Timestamp receiveTime) {
  auto msg = buf.retrieveAsString();
  cout << "recv msg: [" << msg << "] at " << receiveTime.toString() <<  endl;
  cout << "recv size: " << msg.size() << endl;
  if (msg == "\n") {
    conn->shutdown();
  } else {
    conn->send("Hello word!!!");
  }
}
void onWriteComplete(const TcpConnectionPtr& conn) {
  LOG_INFO << "msg write complete";
}

int main() {
  LOG_INFO << "main(): pid = " << ::getpid();
  setLoggerLevel(Logger::TRACE);

  Endpoint listenEndpoint("192.168.158.12", 9981);
  EventLoop loop;
  TcpServer server(&loop, listenEndpoint);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.setWriteCompleteCallback(onWriteComplete);
  server.setThreadNum(4);
  server.start();
  loop.loop();
  
  return 0;
}