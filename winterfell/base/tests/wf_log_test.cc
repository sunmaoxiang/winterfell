/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 15:41:22
 * @Description: 日志库测试代码
 */

#include "winterfell/base/wf_log.h"
using winterfell::setLoggerLevel;

int main() {
  setLoggerLevel(winterfell::Logger::DEBUG);
  LOG_DEBUG("debug print");
  LOG_INFO("info print");
  LOG_WARN("warn prrint");
  LOG_ERROR("error print");

  setLoggerLevel(winterfell::Logger::INFO);
  LOG_DEBUG("debug not print");
  LOG_INFO("info print");
  LOG_WARN("warn prrint");
  LOG_ERROR("error print");

  setLoggerLevel(winterfell::Logger::WARN);
  LOG_DEBUG("debug not print");
  LOG_INFO("info not print");
  LOG_WARN("warn prrint");
  LOG_ERROR("error print");

  setLoggerLevel(winterfell::Logger::FATAL);
  LOG_DEBUG("debug not print");
  LOG_INFO("info not print");
  LOG_WARN("warn not prrint");
  LOG_ERROR("error not print");
  LOG_FATAL("fatal and abort"); //abort
}