/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 11:42:15
 * @Description: 简单的日志库封装：级别 当前时间 线程id 正文 - 源文件名:行号
 */

#pragma once

#include "winterfell/util/wf_singleton.h"
#include "winterfell/util/wf_util.h"

#include <sstream>
#include <string>
#include <stdint.h>
#include <time.h>

using std::string;

namespace winterfell {
class Logger;
class LoggerEvent;
extern Singleton<Logger> g_logger; // 单例模式，全局唯一的日志器

class Logger {
public:
  /**
   * @brief 五种日志等级
  */
  enum LoggerLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
  };

  Logger() = default;

  /**
   * @brief 日志等级的getter,setter方法
  */  
  void setLogLevel(Logger::LoggerLevel level) { level_ = level; }
  Logger::LoggerLevel getLogLevel() const { return level_; }

  /**
   * @brief: 日志输出函数
   * @param {LoggerEvent &} loggerEvent: 日志事件
   * @return {*}
   */  
  void log(const LoggerEvent & loggerEvent);
private:
  LoggerLevel level_;
};

/**
 * @brief: 设置全局的LoggerLevel
 * @param {LoggerLevel} level
 * @return {*}
 */
void setLoggerLevel(Logger::LoggerLevel level);

/**
 * @brief: LoggerLevel的字符串格式，用于日志formmater
 * @param {LoggerLevel} level
 * @return stirng
 */
const string& getLevelString(Logger::LoggerLevel level);

/**
 * 日志事件，主要用于将event转化成指定的formmater
*/
class LoggerEvent {
public:
  explicit LoggerEvent(Logger::LoggerLevel level, time_t t, uint32_t thread_id,
                       const string &msg, const char *fileName, uint32_t lineno);
  const string &fmt() const { return logfmt_;}
  
private:
  string logfmt_;
  
};

};

#define LOG_LEVEL(msg, level) \
  if (winterfell::g_logger.GetInstance()->getLogLevel() <= level) { \
    winterfell::g_logger.GetInstance()->log(winterfell::LoggerEvent(level, time(0), winterfell::GetThreadId(), msg, __FILE__, __LINE__)); \
  }
   
/**
 * @brief 下面是日志输出
*/
#define LOG_DEBUG(msg) LOG_LEVEL(msg, winterfell::Logger::DEBUG)
#define LOG_INFO(msg) LOG_LEVEL(msg, winterfell::Logger::INFO)
#define LOG_WARN(msg) LOG_LEVEL(msg, winterfell::Logger::WARN)
#define LOG_ERROR(msg) LOG_LEVEL(msg, winterfell::Logger::ERROR)
#define LOG_FATAL(msg) LOG_LEVEL(msg, winterfell::Logger::FATAL)
