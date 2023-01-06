/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 20:11:28
 * @Description: 定义各种单独的功能函数，例如获取当前线程ID等
 */

#include <unistd.h>
#include "wf_util.h"

namespace winterfell {

pid_t GetThreadId() {
  return static_cast<pid_t>(syscall(SYS_gettid));
}

};