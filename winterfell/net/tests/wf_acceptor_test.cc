/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-14 11:00:28
 * @Description: 测试acceptor类
 */

#include "winterfell/net/wf_endpoint.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_acceptor.h"
#include "winterfell/base/wf_log.h"
#include <unistd.h>

using namespace std;
using namespace winterfell;

void newConnection(Socket sock, const Endpoint& peerAddr) {
  LOG_INFO << "newConnection(): accepted a new connection from " << peerAddr.getIpPort() ;
  ::write(sock.fd(), "How are you?\n", 13);
}

int main() {
  LOG_INFO << "main() :pid = " << ::getpid();
  Endpoint endpoint(9981);
  EventLoop loop;
  Acceptor acceptor(&loop, endpoint);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();

  loop.loop();
  return 0;
}