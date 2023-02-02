/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-02 16:15:45
 * @Description: 用于实现主从reactor, TcpServer(mainLoop)新建TcpConnection时从SubLoops里挑选一个给TcpConnection用
 */
#include "winterfell/util/wf_noncopyable.h"
#include <memory>
#include <vector>
#include <string>

namespace winterfell {
class Thread;
class EventLoop;
class SubLoops : Noncopyable {
public:
  SubLoops(EventLoop* baseLoop);
  ~SubLoops();
  void setThreadNum(int numThreads) { numThreads_ = numThreads; }
  void start();
  std::shared_ptr<EventLoop> getNextLoop();
  int getId() { return  next_; }
private:
  EventLoop* mainLoop_;  
  bool started_;
  int numThreads_;
  int next_;
  std::vector<std::shared_ptr<Thread> > threads_;
  std::vector<std::shared_ptr<EventLoop>> loops_;
};

}