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
#include <assert.h>
#include <memory>


namespace winterfell {

// __thread关键字是GUN，thread_local是C++11新特性，看起来是全局其实是一个线程一个
__thread EventLoop *t_loopInThisThread = 0;  

EventLoop::EventLoop()
  : looping_(false),
    threadId_(GetThreadId()) {
  LOG_TRACE << "EventLoop create " << this << " in thread " << threadId_;

  // 确保 one loop per thread
  if (t_loopInThisThread) {
    LOG_FATAL << "Another EventLoop exits in this thread";
  }
  else {
    t_loopInThisThread = this;
    poller_.reset( DefaultPoller(t_loopInThisThread) );
  }  
}

EventLoop::~EventLoop() {
  assert(!looping_);
  t_loopInThisThread = NULL;
}

void EventLoop::quit() {
  assert(!quit_);
  quit_ = true;
  
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

};

