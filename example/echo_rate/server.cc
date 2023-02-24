/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-09 15:59:41
 * @Description: 一个Echo服务器，接收到消息原封不动返回
 */

#include "winterfell/net/wf_tcp_server.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_tcp_connection.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/base/wf_rate.h"
#include <string>
#include <unistd.h>

using namespace winterfell;
using namespace std;

rate::Limiter* g_limiter;

void onConnect(const TcpConnectionPtr &conn) {
  if (conn->connected()) {
    LOG_INFO << "onConnect()" << " " << conn->peerEndpoint().getIpPort() ;
  } else {
    abort();
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer& buf, Timestamp timestamp) {
  auto reserv = g_limiter->reserve();
  if (!reserv.OK()) {
    return ;
  } else if (reserv.delay().microSecondsSinceEpoch() > 0){
    LOG_WARN << "流量过大，暂停访问" << reserv.delay().microSecondsSinceEpoch() << "微秒";
    usleep(static_cast<__useconds_t>(reserv.delay().microSecondsSinceEpoch()));
  }
  
  assert(conn->connected());
  string msg = buf.retrieveAsString();
  conn->send(msg);
}

int main() {
  // 设置为每0.1秒添加1个token
  auto limit = rate::Every(Timestamp(static_cast<int64_t>(0.1 * Timestamp::MicroSecondsPerSecond())));
  // 定义每0.1秒添加1个token，令牌桶大小为2
  rate::Limiter limiter(limit,2);
  g_limiter = &limiter;


  setLoggerLevel(Logger::INFO);
  EventLoop loop;
  Endpoint listenEndpoint(8888);
  TcpServer s(&loop, listenEndpoint);
  s.setConnectionCallback(onConnect);
  s.setMessageCallback(onMessage);
  s.setThreadNum(1);
  s.start();
  loop.loop();
  return 0;
}