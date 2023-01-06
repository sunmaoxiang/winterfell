/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 20:57:16
 * @Description: 线程库测试代码
 */

#include "winterfell/base/wf_thread.h"
#include "winterfell/util/wf_util.h"
#include "winterfell/base/wf_log.h"
#include <unistd.h>
#include <iostream>

using winterfell::Thread;
using winterfell::GetThreadId;
using std::cout;
using std::endl;
void thread1() {
  cout << "=====线程1=====" << endl;
}
void thread2() {
  cout << "=====线程2=====" << endl;
}
int main() {
  winterfell::g_logger.GetInstance()->setLogLevel(winterfell::Logger::DEBUG);
  Thread t1(thread1,"thread 1");    
  t1.start();
  Thread t2(thread2, "thread 2");
  t2.start();
  t1.join();
  t2.join();
  
}