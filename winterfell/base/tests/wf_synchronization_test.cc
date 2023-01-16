/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-16 13:28:40
 * @Description: 测试Mutex、Cond
 */

#include "winterfell/base/wf_mutex.h"
#include "winterfell/base/wf_condition.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/base/wf_thread.h"
#include <assert.h>
#include <unistd.h>
using namespace winterfell;

int g_count = 0;
MutexLock gmutex;
Conditon condition(gmutex);
void thread_add() {
  for(int i = 0; i < 1000000; i++) {
    MutexLockGuard t(gmutex);
    g_count = g_count + 1;
  }
}
void thread_sub() {
  for(int i = 0; i < 1000000; i++) {
    MutexLockGuard t(gmutex);
    g_count = g_count - 1;
  }
}

int resouce_count = 0; //用于作为资源计数

void cosumer() {
  while(1) {
    {
      MutexLockGuard t(gmutex);
      while(resouce_count == 0) {
        condition.wait();
      }
    }
    resouce_count--;
    LOG_INFO << "消费一个资源: reouce_count = " <<  resouce_count;
  }
}

void producer() {
  while(1) {
    {
      MutexLockGuard t(gmutex);
      resouce_count++;
      condition.notify();
    }
    sleep(1);
  }
}


int main() {
  // 测试互斥锁
  {
    Thread tadd(thread_add);
    Thread tsub(thread_sub);
    tadd.start();
    tsub.start();
    tadd.join(); tsub.join();
    assert( g_count == 0 );
  }
  
  // 多生产者单消费者测试cond
  {
    Thread c(cosumer); 
    Thread p0(producer);
    Thread p1(producer);
    Thread p2(producer);

    c.start(); p1.start(); p2.start();

    c.join(); p1.join(); p2.join();
  }
  return 0;
}