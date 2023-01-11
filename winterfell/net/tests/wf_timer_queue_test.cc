/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 21:40:11
 * @Description: 时间队列测试
 */


#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_timer_queue.h"
#include "winterfell/base/wf_log.h"
#include <functional>
using namespace winterfell;

void print(const char* msg) {
  LOG_INFO << msg;
}

int main() {
  setLoggerLevel( Logger::INFO );
  LOG_INFO << "now";
  EventLoop loop;
  loop.runAt(addTime(Timestamp::now(), 5), std::bind(print, "runAt 5 s") );
  loop.runEvery(3, std::bind(print,"runEvery 3 s"));
  loop.runAfter(4, std::bind(print,"runAfter 4 s"));
  loop.loop();
  return 0;
}