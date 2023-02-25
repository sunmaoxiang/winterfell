/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 21:33:39
 * @Description: 继承自Poller，调用底层<poll.h>
 */

#include "wf_poll_poller.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_channel.h"
#include <poll.h>
#include <cassert>


namespace winterfell {

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop)
{
}

PollPoller::~PollPoller() = default;

Timestamp PollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
  int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
  Timestamp now(Timestamp::now());
  if (numEvents > 0) {
    LOG_TRACE << numEvents << " events happended";
    fillActiveChannels(numEvents, activeChannels);
  } else if (numEvents == 0) {
    // LOG_TRACE << " nothing happended";
  } else {
    LOG_ERROR << "PollPoller::poll" ;
  }
  return now;
}

void PollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
  for(PollFdList::const_iterator pfd = pollfds_.begin();
      pfd != pollfds_.end() && numEvents > 0; ++pfd) {
    if(pfd->revents > 0) {
      --numEvents;
      ChannelMap::const_iterator ch = channels_.find(pfd->fd);
      assert(ch != channels_.end());
      Channel* channel = ch->second;
      assert(channel->fd() == pfd->fd);
      channel->set_revents(pfd->revents);
      activeChannels->push_back(channel);
    }    
  }
}

void PollPoller::updateChannel(Channel* channel) {
  assertInLoopThread();
  LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
  if (channel->index() < 0) {
    // 之前未添加过，index初始化为-1
    assert(channels_.find(channel->fd()) == channels_.end());
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size()) - 1;
    channel->set_index(idx);
    channels_[pfd.fd] = channel;
  } else {
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    auto idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>( pollfds_.size() ));
    struct pollfd& pfd = pollfds_[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -1);
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if(channel->isNoneEvent()) {
      pfd.fd = -1;
    }
  }
}

void PollPoller::removeChannel(Channel* channel) {
  assertInLoopThread();
  LOG_TRACE << "remove channel fd = " << channel->fd() << " events = " << channel->events();
  if (channel->index() < 0) {
    assert(channels_.find(channel->fd()) == channels_.end());
    LOG_TRACE << "channel fd = " << channel->fd() << " not in pollfds";
  } else {
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    channels_.erase(channel->fd()); // 从 map<fd, Channel*>中移除指定channel
    int idx = channel->index();
    // pollfds_.erase(pollfds_.begin() + idx); O(N)
    /* we need O(1) */
    int channelAtEnd = pollfds_.back().fd;
    if (channel->fd() != channelAtEnd) {
      channels_[channelAtEnd]->set_index(idx);
      std::iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
    }
    pollfds_.pop_back();
  }
}
}; // namespace 
