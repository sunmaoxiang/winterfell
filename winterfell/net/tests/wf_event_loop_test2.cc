/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-08 15:44:05
 * @Description: 事件监听循环，测试使用Channel添加新的监听事件 
 */

#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include "winterfell/net/wf_channel.h"

#include <sys/timerfd.h>
#include <cstring>
#include <unistd.h>
winterfell::EventLoop* g_loop;

void timeout() {
  LOG_INFO << "TimeOut!" ;
  g_loop->quit();
}


int main() {
  winterfell::setLoggerLevel(winterfell::Logger::TRACE);  
  g_loop = new winterfell::EventLoop();

  /**
   * CLOCK_MONOTONIC:单调递增
   * TFD_NONBLOCK: 非阻塞IO
   * 当使用exec系统调用时自动关闭
   */
  int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  winterfell::Channel channel(g_loop, timerfd);

  /**
   * 设置读监听IO事件
   */
  channel.setReadCallback(timeout); 
  channel.enableReading();

  struct itimerspec howlong;
  memset(&howlong, 0, sizeof howlong);

  howlong.it_value.tv_sec = 5; // 定时为5秒
  timerfd_settime(timerfd, 0, &howlong, NULL);

  g_loop->loop();

  close(timerfd);
}