/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-16 13:13:11
 * @Description: 封装条件变量
 */

#pragma once
#include "winterfell/base/wf_mutex.h"
#include <pthread.h>
namespace winterfell {

class Conditon : Noncopyable {

public:
  Conditon(MutexLock &mutex) :mutex_(mutex) { 
    ::pthread_cond_init(&cond_, NULL);
  }

  void wait() {
    ::pthread_cond_wait(&cond_, &mutex_.getPthreadMutex());
  }

  void notify() {
    ::pthread_cond_signal(&cond_);
  }

  void notifyAll() {
    ::pthread_cond_broadcast(&cond_);
  }

private:
  MutexLock &mutex_;
  pthread_cond_t cond_;
};
}