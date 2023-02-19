/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-17 21:19:29
 * @Description: 实现限流器功能
 */

#pragma once

#include <float.h>
#include <winterfell/base/wf_mutex.h>
#include <winterfell/base/wf_timestamp.h>
namespace winterfell {
namespace rate {
  class Reservation;

  // Limit is represented as number of events per second.
  typedef double Limit;
  const Limit Inf = DBL_MAX;
  Timestamp durationFromTokens(Limit limit, double tokens);
  // Every converts a minimum time interval between events to a Limit.
  Limit Every(Timestamp interval);
  class Limiter {
  public:
    Limiter(Limit r, int b)
    : limit_(r),
      burst_(b),
      tokens_(0.0),
      last_(0)
    {}
    Limit limit();
    int burst();
    // advance calculates and returns an updated state for lim resulting from the passage of time.
    double advance(Timestamp t);
    // tokensAt returns the number of tokens available at time t
    double tokensAt(Timestamp t);
    // tokens returns the number of tokens avilable now
    double tokens();
    // 当事件发生时，令牌数会-1，然后返回这个时间能否现在发生，如果不能立刻发生需要等待多长时间，以及如果不想让这件事发生如何恢复(调用cancel)
    Reservation reserve();
    Reservation reserveN(int n);
    void setLast(Timestamp last) { last_=last; } 
  private:
    MutexLock mutex_;
    Limit limit_; 
    int burst_; // 令牌桶容量
    double tokens_; // 桶中目前剩余的token数目，可以为负数
    // last is the last time the limiter's tokens field was updated
    Timestamp last_;
  };

  // A Reservation holds information about events that are permitted by a Limiter to happen after a delay.
  // A Reservation may be canceled, which may enable the Limiter to permit additional events.
  class Reservation {
  public:
    Reservation(bool ok, Limiter* lim, int tokens, Timestamp timeToAct) 
    : ok_(ok),
    lim_(lim),
    tokens_(tokens),
    timeToAct_(timeToAct){
    }
    // 是否能立即发生
    bool OK() const { return ok_; }
    // 如果发生需要等待多长时间能拿到token
    Timestamp delay();
    // 恢复token
    void cancel(); // TODO
  private:
    bool ok_;
    Limiter* lim_;
    int tokens_;
    Timestamp timeToAct_;
  };


}
}