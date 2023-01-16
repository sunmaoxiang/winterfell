/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-16 09:32:58
 * @Description: 封装pthread_init, pthread_mutex_destroy, pthread_mutex_lock, pthread_mutex_unlock
 */

#pragma once

#include "winterfell/base/wf_log.h"
#include "winterfell/util/wf_util.h"
#include "winterfell/util/wf_noncopyable.h"

#include <pthread.h>
#include <cassert>

namespace winterfell {
class MutexLock : Noncopyable {
public:
  MutexLock() : holder_(GetThreadId()) {
    ::pthread_mutex_init(&mutex_, NULL);
  }
  ~MutexLock() {
    ::pthread_mutex_destroy(&mutex_);
  }
  void lock() {
    holder_ = GetThreadId();
    ::pthread_mutex_lock(&mutex_);
  } 
  void unlock() {
    holder_ = 0;
    ::pthread_mutex_unlock(&mutex_);
  }
  /**
   * @brief 可以判断这个锁是否被这个线程使用
  */
  bool isLockeddByThisThread() {
    return holder_ == GetThreadId();
  }
  void assertIsLockeddByThisThread() {
    assert(isLockeddByThisThread());
  }

  pthread_mutex_t& getPthreadMutex()  {
    return mutex_;
  }
private:
  pthread_mutex_t mutex_;
  int holder_;
};

class MutexLockGuard {
public:
  MutexLockGuard(MutexLock& mutex) : mutex_(mutex) { 
    mutex_.lock();
  } 
  ~MutexLockGuard() {
    mutex_.unlock();
  }
private:
  MutexLock& mutex_;
};

}
