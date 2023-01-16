/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 09:07:07
 * @Description: 定义事件监听循环
 */

#include "wf_event_loop.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_poller.h"
#include "winterfell/net/poller/wf_default_poller.h"
#include "winterfell/net/wf_channel.h"
#include "winterfell/net/wf_timer.h"
#include "winterfell/net/wf_timer_queue.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <assert.h>
#include <memory>



namespace winterfell {

// __thread关键字是GUN，thread_local是C++11新特性，看起来是全局其实是一个线程一个
__thread EventLoop *t_loopInThisThread = 0;  

EventLoop::EventLoop()
  : looping_(false),
    threadId_(GetThreadId()), 
    poller_( DefaultPoller(this) ),
    timerQueue_(std::unique_ptr<TimerQueue>(new TimerQueue(this))),
    eventFd(createEventFd()), 
    wakeupChannle_(std::unique_ptr<Channel>(new Channel(this, eventFd))) {
    LOG_TRACE << "EventLoop create " << this << " in thread " << threadId_;

  // 确保 one loop per thread
  if (t_loopInThisThread) {
    LOG_FATAL << "Another EventLoop exits in this thread";
  }
  else {
    t_loopInThisThread = this;
    wakeupChannle_->setReadCallback( std::bind(&EventLoop::handleRead, this) );
    wakeupChannle_->enableReading();  
  }  
}

EventLoop::~EventLoop() {
  assert(!looping_);
  t_loopInThisThread = NULL;
}


void EventLoop::loop() {
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  quit_ = false;
  
  while(!quit_) {
    activeChannels_.clear();
    poller_->poll(kPollTimeMs, &activeChannels_);
    for(auto &it : activeChannels_) {
      it->handleEvent();
    }
    doPendingFunctors();
  }

  LOG_TRACE << "EventLoop " << this << " stop looping";
  looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " << GetThreadId();
}

void EventLoop::updateChannel(Channel* channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
}

void EventLoop::runAt(Timestamp at, TimerCallback tcb) {
  timerQueue_->addTimer(new Timer(at, 0, tcb));
}
void EventLoop::runAfter(int64_t delta, TimerCallback tcb) {
  auto at = addTime(Timestamp::now(), delta); 
  timerQueue_->addTimer(new Timer(at, 0, tcb ));
}
void EventLoop::runEvery(int64_t interval, TimerCallback tcb) {
  auto at = addTime(Timestamp::now(), interval);
  timerQueue_->addTimer(new Timer(at, interval, tcb));
}
void EventLoop::runInLoop(const Functor &cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(cb);   
  }
}
void EventLoop::queueInLoop(const Functor &cb) {
  {
    MutexLockGuard lock(mutex_);
    pendingFunctors_.push_back(cb);
  }
  // callingPendingFunctors_作用是在functor本身有添加functor的行为时可以唤醒
  if (!isInLoopThread() || callingPendingFunctors_) {
    wakeup();
  }
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;
  // 使用swap来减少临界区大小
  {
    MutexLockGuard lock(mutex_);
    functors.swap(pendingFunctors_);
  }
  for(auto &functor: functors) {
    functor();
  }
  callingPendingFunctors_ = false;
}

void EventLoop::quit() {
  quit_ = true;
  if (!isInLoopThread()) {
    wakeup();
  }
}

void EventLoop::wakeup() {
  uint64_t e = 1;
  ::write( eventFd, &e, sizeof e);
}

void EventLoop::handleRead() {
  uint64_t e = 0;
  ::read(eventFd, &e, sizeof e);    
}

int EventLoop::createEventFd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    LOG_FATAL << "Failed in eventfd";
  }
  return evtfd;
}
}

