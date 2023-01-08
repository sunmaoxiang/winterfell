/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 15:48:18
 * @Description: 事件循环测试代码，事件是空的，开两个线程，分别是one loop per thread
 */


#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/base/wf_thread.h"
#include "winterfell/util/wf_util.h"


#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

void threadFunc() 
{
  LOG_INFO << "threadFunc(): pid = " << getpid() << ", tid = " <<  winterfell::GetThreadId() ;

  winterfell::EventLoop loop;
  loop.loop();
}

int main() {
  LOG_INFO << "threadFunc(): pid = " << getpid() << ", tid = " << winterfell::GetThreadId();
  winterfell::EventLoop loop;
  

  winterfell::Thread thread(threadFunc);
  thread.start();

  loop.loop();

  thread.join();
  return 0;
}