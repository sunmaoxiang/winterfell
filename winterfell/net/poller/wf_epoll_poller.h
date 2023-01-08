/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 21:21:49
 * @Description: 继承自Poller，调用底层<sys/epoll.h>
 */

#pragma once

#include "winterfell/net/wf_poller.h"

#include <vector>

struct epoll_event;

namespace winterfell {

class EpollPoller : public Poller {
public:
  EpollPoller(EventLoop* loop);
  ~EpollPoller() override;

  Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
  void updateChannel(Channel* channel) override;
  void removeChannel(Channel* channel) override;

private:
  static const int kInitEventListSize = 16; // 红黑树初始节点数
  static const char* operationToString(int op);
  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
  void update(int operation, Channel* channel);

  typedef std::vector<struct epoll_event> EventList;

  int epollfd_;
  EventLoop events_;
};

};