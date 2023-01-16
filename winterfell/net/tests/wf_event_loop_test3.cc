/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-16 16:40:11
 * @Description: 测试runInLoop功能，开两个线程，一个是EventLoop，一个是往里面放functor的线程
 */

#include "winterfell/net/wf_event_loop.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/base/wf_thread.h"
#include <unistd.h>
using namespace winterfell;
EventLoop* g_loop;

void functor() {
  LOG_INFO << "thread Add";
}

void threadAdd() {
  while(1) {
    g_loop->runInLoop(functor);
    sleep(1);
  }
}


int main() {
  Thread add(threadAdd);  

  EventLoop loop;
  g_loop = &loop;
  add.start();
  loop.loop();
}