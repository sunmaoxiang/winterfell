/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 17:30:30
 * @Description: Poller抽象基类，可以derive出支出poll和epoll两种子类
 */

#pragma once

#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/base/wf_timestamp.h"
#include "winterfell/net/wf_event_loop.h"
#include <vector>
#include <map>


namespace winterfell {
class Channel;


class Poller : Noncopyable {
public:
  typedef std::vector<Channel*> ChannelList;

  Poller(EventLoop* loop);
  virtual ~Poller();
  /**
   * @brief: 必须在loop thread中调用，然后会监听IO事件(只要有一个纯虚函数就是抽象基类，然后基类能实例化)
   * @param {int} timeoutMs
   * @param {ChannelList*} activeChannels
   * @return {Timestamp} 
   */
  virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

  /**
   * @brief: 改变监听的IO事件 通过EventLoop::update()调用
   * @param {Channel*} channel
   * @return {*}
   */  
  virtual void updateChannel(Channel* channel) = 0;

  /**
   * @brief: 只有channel destructs才能remove掉，不然会出现悬挂指针
   * @param {Channel*} channel
   * @return {*}
   */  
  virtual void removeChannel(Channel* channel) = 0;
  /**
   * @brief: 判断某个事件监听器是否在List中
   * @param {Channel*} channel
   * @return {*}
   */
  virtual bool hasChannel(Channel* channel) const; 
  
  /**
   * @brief: 返回一个默认的poller(poll or epoll其中一个)，并注册到loop中
   * @param {EventLoop*} loop
   * @return Poller
   */  
  static Poller* newDefaultPoller(EventLoop* loop);

  void assertInLoopThread() const {
    ownerLoop_->assertInLoopThread();
  }


protected: 
  typedef std::map<int, Channel*> ChannelMap;
  ChannelMap channels_;

private:
  EventLoop* ownerLoop_;
};
};