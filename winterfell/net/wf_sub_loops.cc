/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-02 16:15:54
 * @Description: 用于实现主从reactor, TcpServer(mainReactor)新建TcpConnection时从SubLoops里挑选一个给TcpConnection用
 */
#include "winterfell/net/wf_sub_loops.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/base/wf_thread.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/base/wf_thread.h"
#include <unistd.h>
namespace winterfell {
SubLoops::SubLoops(EventLoop *mainLoop)
: mainLoop_(mainLoop),
  started_(false),
  numThreads_(1),
  next_(0) {
}
SubLoops::~SubLoops() = default;
void SubLoops::start() {
  assert(!started_);

  for (int i = 0; i < numThreads_; i++) {
    
    auto threadPtr = std::shared_ptr<Thread>(new Thread([&]() {
      auto loopPtr = std::shared_ptr<EventLoop>(new EventLoop());
      loops_.push_back(loopPtr);
      loopPtr->loop();
    }));

    threads_.push_back(threadPtr);
    threadPtr->start();
  }
  started_ = true;  
}
std::shared_ptr<EventLoop> SubLoops::getNextLoop() {
  next_ = (next_+1) % numThreads_;
  return loops_[next_];
}
}