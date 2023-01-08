/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 17:01:10
 * @Description: Channel类定义，负责一个fd的IO事件分发
 */

#include "winterfell/net/wf_channel.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include <poll.h>

namespace winterfell {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fdArg)
  : loop_(loop),
    fd_(fdArg),
    events_(0),
    revents_(0),
    index_(-1) {
}


void Channel::update() {
  loop_->updateChannel(this);
}

void Channel::handleEvent() {
  // POLLNVAL Invalid request: fd not open (output only).
  if (revents_ & POLLNVAL) {
    LOG_WARN << "Channel::handleEvent() POLLINVAL: fd not open";
  }
  // POLLERR: Error condition
  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }
  /**
   * POLLIN: socket有数据可读
   * POLLPRI:  In TCP frame header, there is a flag named urg_data. Urg_data means this frame has higher priority to delivery.
   * POLLHUP: socket的另一端关闭时,或读到文件结尾
   */
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if(readCallback_) readCallback_();
  }
  // POLLOUT: Writing now will not block.
  if (revents_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }
}

};