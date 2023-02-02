/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 14:59:50
 * @Description: 定时器类声明，只支持second单位的计时
 */

#pragma once
#include "winterfell/base/wf_timestamp.h"
#include "winterfell/util/wf_callbacks.h"
#include <functional>
#include <memory>

namespace winterfell {

class Timer {
public:
  typedef std::unique_ptr<Timer> Ptr;

  Timer(Timestamp when, double interval_, TimerCallback& tcb);
  ~Timer() = default;
  /**
   * @brief: 用于TimerQueue调用
   * @return {*}
   */  
  void runTcb();
  
  /**
   * @brief 一系列getter
  */
  Timestamp when() const { return when_; }
  int repeated() const { return repeaded_; }
  double interval() const { return interval_; } 

  /**
   * @brief 更新when，用于重新计时tricker
  */
  void updateWhen();

private:  
  Timestamp when_;            // 当前计时器被唤醒的时间
  bool repeaded_;             // 是否是重复的，用于EventLoop::runEvery()
  double interval_;          // <= 0 不发生重复
  TimerCallback tcb_;         // 用于TimerQueue调用
};

}
