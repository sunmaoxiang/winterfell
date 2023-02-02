/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-08 15:13:06
 * @Description: 测试Client
 */
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_tcp_client.h"

using namespace winterfell;
void onConnection(const TcpConnectionPtr& conn) {
  LOG_INFO << "new Connection:" << conn->peerEndpoint().getIpPort();
  conn->send("smx");
}
void onMessage(const TcpConnectionPtr &conn, Buffer &buf)
{
  auto msg = buf.retrieveAsString();
  LOG_INFO << msg ;
}
void onWriteComplete(const TcpConnectionPtr &conn) {
  LOG_INFO << "writeComplete";
}
int main() {
  EventLoop loop;
  Endpoint endPoint("localhost", 9981);
  TcpClient client(&loop, endPoint, "TcpClient");
  
  client.setConnectionCallback(onConnection);
  client.setMessageCallback(onMessage);
  client.setWriteCompleteCallback(onWriteComplete);
  client.connect();
  loop.loop();
}