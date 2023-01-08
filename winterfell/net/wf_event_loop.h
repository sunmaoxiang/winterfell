/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 08:47:54
 * @Description: 声明事件监听循环
 */
#pragma once

#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/util/wf_util.h"


#include <pthread.h>
#include <vector>
#include <memory>

namespace winterfell {

class Channel;
class Poller;

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
private:
  void abortNotInLoopThread();
  bool looping_;
  const pid_t threadId_;
  typedef std::vector<Channel*> ChannelList;
  bool quit_;
  ChannelList activeChannels_;
  std::unique_ptr<Poller> poller_; // 一个event_loop独占一个poller
  const static int kPollTimeMs = 5 * 1000;
};

}

