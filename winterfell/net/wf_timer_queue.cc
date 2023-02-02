/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 15:41:58
 * @Description: 事件队列定义，主要用于添加Timer定时器(会在EventLoop中注册timer_fd)
 */


#include "wf_timer_queue.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_channel.h"
#include "winterfell/base/wf_log.h"

#include <sys/timerfd.h>
#include <cstring>
#include <unistd.h>
#include <utility>

namespace winterfell
{

int TimerQueue::create_timefd()
{
  int timefd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timefd < 0)
  {
    LOG_FATAL << "create new timefd failed";
  }
  return timefd;
}

TimerQueue::TimerQueue(EventLoop * loop)
    : loop_(loop),
      timerfd_(create_timefd()),
      channel_(new Channel(loop, timerfd_))
{
  channel_->setReadCallback(std::bind(&TimerQueue::timerQueueCallBack, this));
  channel_->enableReading();
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
  // channel_ 需要由EventLoop回收
}

void TimerQueue::addTimer(Timer* timer)
{
  loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer)); 
}

void TimerQueue::addTimerInLoop(Timer *timer) {
  struct itimerspec howlong;
  memset(&howlong, 0, sizeof howlong);
  int64_t microseconds = timer->when().microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
  howlong.it_value.tv_sec = microseconds / Timestamp::MicroSecondsPerSecond();
  howlong.it_value.tv_nsec = (microseconds % Timestamp::MicroSecondsPerSecond()) * 1000;
  timerfd_settime(timerfd_, 0, &howlong, NULL);
  timerHeap_.insert({timer->when(), timer});
}

void TimerQueue::timerQueueCallBack()
{
  // 把timefd_中的信息读出来，因为poller是LT，不读出来的话会一直触发
  uint64_t exp; // number of expirations that have occurred
  ::read(timerfd_, &exp, sizeof(exp));
  TimerHeap::iterator expEnd = timerHeap_.lower_bound(Timestamp::now()); // 最后一个超时的事件
  vector<Entry> expvec(timerHeap_.begin(), expEnd);
  timerHeap_.erase(timerHeap_.begin(), expEnd);
  reset(expvec.begin(), expvec.end());
}

void TimerQueue::reset(vector<Entry>::iterator expirationBegin, vector<Entry>::iterator expirationEnd)
{
  for (auto & it = expirationBegin; it != expirationEnd; it++)
  {
    it->second->runTcb();
    if (it->second->repeated())
    {
      it->second->updateWhen();
      addTimer(it->second);
    }
  }
}

}