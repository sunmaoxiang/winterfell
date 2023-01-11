/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 19:25:14
 * @Description: typdedef所有callbacks
 */

#pragma once

#include <functional>

namespace winterfell {
  /**
   * @brief: 用于注册到Channel
   */  
  typedef std::function<void()> EventCallback;

  /**
   * @brief 用于注册runAt, runAfter, runEvery
  */
  typedef std::function<void()> TimerCallback;
}