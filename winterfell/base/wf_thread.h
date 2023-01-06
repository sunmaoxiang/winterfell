/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 11:40:48
 * @Description: 线程库声明
 */
#pragma once

#include "winterfell/util/wf_noncopyable.h"

#include <functional>
#include <string>

#include <pthread.h>

using std::string;

namespace winterfell {

class Thread : Noncopyable {
public:
  typedef std::function<void()> ThreadFunc;
  
  /**
   * @brief: 显式构造函数，防止出现 Thread t = { threadFunc, name };
   * @param {ThreadFunc&&} threadFunc
   * @param {string&} name
   * @return {*}
   */  
  explicit Thread(ThreadFunc&& threadFunc, const string& name = string());
  
  ~Thread();
  
  /**
   * @brief: 启动新线程
   * @return {*}
   */  
  void start();

  /**
   * @brief 用于回收线程资源
   * @return pthread_join返回值
   */  
  int join();

  /**
   * @brief 用于执行threadFunc
  */
  void runInThread() { func_(); }

  /**
   * @brief 一系列getter方法
  */
  bool started() const { return started_; }
  pid_t tid() const { return tid_; }
  const string &name() const { return name_; }

private:
  bool started_; // 线程是否启动
  bool joined_;  // 线程是否被回收
  pthread_t pthreadId_; // 线程标识符: pthread库使用其标识唯一线程
  pid_t tid_;     // 线程id: 系统中标识线程
  ThreadFunc func_; // 线程执行流
  string name_; // 线程名称
};

};
