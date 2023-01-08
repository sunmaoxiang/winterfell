/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-08 15:18:30
 * @Description: Poller抽象基类，可以derive出支出poll和epoll两种子类的公共实现
 */

#include "winterfell/net/wf_poller.h"
#include "winterfell/net/wf_channel.h"
namespace winterfell {

Poller::Poller(EventLoop* loop): ownerLoop_(loop) {}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel* channel) const {
  return channels_.find(channel->fd()) != channels_.end();
}

}