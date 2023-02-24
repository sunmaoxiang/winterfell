/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-19 14:02:14
 * @Description: 测试限流器客户端
 */

#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_tcp_client.h"
#include "winterfell/base/wf_timestamp.h"
#include <unistd.h>

using namespace winterfell;
void onConnection(const TcpConnectionPtr &conn)
{
  LOG_INFO << "new Connection:" << conn->peerEndpoint().getIpPort();
  conn->send("smx");
}
void onMessage(const TcpConnectionPtr &conn, Buffer &buf, Timestamp timestamp)
{
  auto msg = buf.retrieveAsString();
  LOG_INFO << "receive " <<  msg << " at " << timestamp.toString();
}
void onWriteComplete(const TcpConnectionPtr &conn)
{
  // usleep(200 * 1000); 不会发生限流
  // usleep(99 * 1000);// 轻微限流
  usleep(1 * 1000); // 大量限流
  LOG_INFO << "writeComplete";
  conn->send("smx");
}
int main()
{
  EventLoop loop;
  Endpoint endPoint("localhost", 8888);
  TcpClient client(&loop, endPoint, "TcpClient");

  client.setConnectionCallback(onConnection);
  client.setMessageCallback(onMessage);
  client.setWriteCompleteCallback(onWriteComplete);
  client.connect();
  loop.loop();
}