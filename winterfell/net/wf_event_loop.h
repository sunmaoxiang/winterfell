/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 08:47:54
 * @Description: 声明事件监听循环
 */
#pragma once

#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/util/wf_util.h"
#include "winterfell/util/wf_callbacks.h"
#include "winterfell/base/wf_mutex.h"


#include <pthread.h>
#include <vector>
#include <memory>

namespace winterfell {

class Channel;
class Poller;
class Timestamp;
class TimerQueue;

class EventLoop : Noncopyable {
public:
  
  EventLoop();
  ~EventLoop();

  /**
   * @brief: 启动事件循环
   * @return {*}
   */  
  void loop();

  /**
   * @brief: 用于断言是否是one loop per thread
   * @return {*}
   */
  void assertInLoopThread() {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() const { return threadId_ ==  GetThreadId(); }

  /**
   * @brief 用于退出事件监听循环
  */
  void quit();


  /**
   * @brief 添加 or 修改 loop中监听的IO事件
  */
  void updateChannel(Channel* channel);

  /**
   * @brief runAt: 在指定事件启动事件
  */
  void runAt(Timestamp at, TimerCallback tcb);
  /**
   * @brief runAfter: 在 delta seconds 之后执行某个事件
  */
  void runAfter(int64_t delta, TimerCallback tcb);
  /**
   * @brief runEvery: 每到interval间隔事件后执行某个事件
  */
  void runEvery(int64_t interval, TimerCallback tcb);

  /**
   * @brief 在本线程内执行某个用户的任务回调，当是本线程调用时是同步执行（原因是单线程无锁），当是别的线程调用需要加入任务队列中，在唤醒poller后调用
  */
  void runInLoop(const Functor& cb);
  void queueInLoop(const Functor& cb);

private:
  void abortNotInLoopThread();
  bool looping_;
  const pid_t threadId_;
  typedef std::vector<Channel*> ChannelList;
  bool quit_;
  ChannelList activeChannels_;
  std::unique_ptr<Poller> poller_; // 一个event_loop独占一个poller
  std::unique_ptr<TimerQueue> timerQueue_; // 一个event_loop独占一个定时器队列
  const static int kPollTimeMs = 5 * 1000;

  std::vector<Functor> pendingFunctors_;  // @GuardeBy mutex_
  MutexLock mutex_;
  bool callingPendingFunctors_;
  int eventFd;
  std::unique_ptr<Channel> wakeupChannle_; // 用于监听有事件到来，需要执行pendingFunctors_中的回调函数
  void handleRead();
  void doPendingFunctors();
  void wakeup();
  int createEventFd();
};

}

