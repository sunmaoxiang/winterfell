/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 13:37:40
 * @Description: 线程库定义
 */

#include "wf_thread.h"
#include "wf_log.h"
#include <string>
#include <assert.h>

namespace winterfell {


Thread::Thread(ThreadFunc &&threadFunc, const string &n) 
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(threadFunc),
    name_(n) {
}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadId_);
  }
}
void *startThread(void *obj) {
  Thread *thread = static_cast<Thread *>(obj);
  thread->runInThread();
  return NULL;
}

void Thread::start() {
  assert(!started_);   
  started_ = true;
  
  if(pthread_create(&pthreadId_, NULL, startThread, this)) {
    started_ = false;
    LOG_FATAL("Failed in pthread_create");
  } else {
    LOG_DEBUG(string("线程启动, ID = ") + std::to_string(tid_));
  }
}

int Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  LOG_DEBUG(string("回收线程, ID = ") + std::to_string(tid_));
  return pthread_join(pthreadId_, NULL);
}

};