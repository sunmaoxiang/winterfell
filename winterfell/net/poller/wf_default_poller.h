/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-08 14:16:25
 * @Description: 返回默认的poller，现在只实现了pollpoller
 */

#include "winterfell/net/wf_poller.h"
#include "winterfell/net/poller/wf_poll_poller.h"
#include "winterfell/net/wf_event_loop.h"

namespace winterfell {

inline Poller* DefaultPoller( EventLoop* loop ) {
  return new PollPoller( loop );  
}

} // namespace winterfell

