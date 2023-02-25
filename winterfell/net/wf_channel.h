/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 16:09:37
 * @Description: Channel类声明，负责一个fd的IO事件分发
 */
#pragma once

#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/util/wf_callbacks.h"

namespace winterfell {

class EventLoop;

class Channel : Noncopyable {
public:
  typedef std::function<void(Timestamp)> ReadEventCallback;
  /**
   * @brief: 构造函数
   * @param {EventLoop*} loop channel需要注册进的事件循环
   * @param {int} fd 需要监听的文件描述符
   * @return {*}
   */  
  Channel(EventLoop* loop, int fd);
  ~Channel(); 
  /**
   * @brief handleEvent()是Channel的核心，它由EventLoop::loop()调用，根据revents_值分别调用不同的回调函数
  */
  void handleEvent(Timestamp receiveTime);

  /**
   * @brief 三种事件回调函数的setter, 当fd上出现可读，可写，出错，调用相关函数
  */
  void setReadCallback(const ReadEventCallback& cb) { readCallback_ = cb; }
  void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
  void setErrorCallback(const EventCallback& cb) { errorCallback_ = cb; }
  void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }
  
  /**
   * @brief 返回channel管理的fd
  */
  int fd() const {return fd_;}

  /**
   * @brief 返回要监听的事件mask，二进制表示
  */
  int events() const { return events_; }

  /**
   * 设置需要监听的IO事件
  */
  void enableReading() { events_ |= kReadEvent; update(); }
  void enableWriting() { events_ |= kWriteEvent; update(); }
  void disableWriting() { events_ &= ~kWriteEvent; update(); }
  void disableAll() { events_ |= kNoneEvent; update(); }
  bool isWriting() const { return events_ & kWriteEvent; }
  
  /**
   * @brief 设置已经返回的事件mask
  */
  void set_revents(int revt) { revents_ = revt; }

  /**
   * @brief 返回已经发生的事件类型
  */
  int revents() const { return revents_; }

  /**
   * @brief 判断监听的事件是否为空
  */
  bool isNoneEvent() const { return events_ == kNoneEvent; }

  /**
   * @brief 对 poller 有用，用来记录监听的fd在整个fds数组中的位置
  */
  int index() { return index_; }
  void set_index(int indexArg) { index_ = indexArg; }  
  
  /**
   * @return channel 的 owner loop
  */
  EventLoop* ownerLoop() { return loop_; }

  /**
   * @brief 用于从eventLoop中移除该channel
  */
  void remove();
  
  private:
    void update(); // update() ->  Eventloop::updateChannel -> Poller::updateChannel

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_; // channel所在的eventloop
    

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

    const int fd_; // 构造函数成员列表初始化
    int events_;   // 关心的IO事件
    int revents_;  // 目前活动的事件，由poller设置
    int index_;    

    // bool eventHandling_;
};

};
