/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 15:41:49
 * @Description: 事件队列声明，主要用于添加Timer定时器(会在EventLoop中注册timer_fd)
 */

#pragma once

#include <map>
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/base/wf_timestamp.h"
#include "wf_timer.h"
#include <vector>
#include "memory"

namespace winterfell {
class Channel;
using std::vector;
class TimerQueue { 
public:
  typedef std::multimap<Timestamp, Timer*> TimerHeap; 
  typedef std::pair<Timestamp, Timer*> Entry; 

  TimerQueue(EventLoop* loop);
  ~TimerQueue();
 
  /**
   * @brief: 添加一个Timer，先在eventloop注册这一个事件，然后在TimerHeap中添加这一个计时器
   * @param {Ptr} timer
   * @return {*}
   */    
  void addTimer(Timer *timer);

  /**
   * @brief: 注册到EventLoop中的事件
   * @return {*}
   */  
  void timerQueueCallBack();

private:
  void addTimerInLoop(Timer* timer);
  /**
   * @brief: 
   * @param [expirationBegin, expirationEnd) 为TimerHeap中所有过期的Timer
   * @return {*}
   */
  void reset(vector<Entry>::iterator expirationBegin, vector<Entry>::iterator expirationEnd);

  /**
   * @brief: 用于创建一个新的时间监听描述符
   * @return int
   */  
  int create_timefd();

  TimerHeap timerHeap_; // 用于管理所有Timer，并在到期时更新timefd状态

  EventLoop *loop_;      // 用loop监听timefd事件
  int timerfd_;

  Channel *channel_;      // 将timefd_和timerQueueCallBack封装到其中，只有指针才可以用forward declare


};

}