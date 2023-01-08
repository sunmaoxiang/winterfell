/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 21:34:40
 * @Description: 继承自Poller，调用底层<poll.h>
 */

#include "winterfell/net/wf_poller.h"
#pragma once

struct pollfd;
namespace winterfell {


class PollPoller : public Poller
{
public:
  PollPoller(EventLoop *loop);
  ~PollPoller() override;

  Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
  void updateChannel(Channel *channel) override;
  void removeChannel(Channel *channel) override;

private:
  void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
  
  typedef std::vector<struct pollfd> PollFdList;
  
  PollFdList pollfds_;  
};

};