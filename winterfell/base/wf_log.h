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
#include <memory>
#include <stdint.h>
#include <time.h>


using std::string;
using std::stringstream;

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
    TRACE,
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
 * @brief 日志事件，主要用于将event转化成指定的formmater，然后与WrapperLogger会将LogggerEvent与Logger组装成将要输出的日志
*/
class LoggerEvent {
public:

  /**
   * @brief: 构造函数
   * @param {LoggerLevel} level 日志等级
   * @param {time_t} t 当前时间
   * @param {uint32_t} thread_id 线程ID
   * @param {char} *fileName 当前文件名
   * @param {uint32_t} lineno 当前行号
   * @return {*}
   */  
  explicit LoggerEvent(Logger::LoggerLevel level, time_t t, uint32_t thread_id, const char *fileName, uint32_t lineno);
  /**
   * @brief 级别 当前时间 线程id %s - 源文件名:行号, 其中 %s 是正文部分
   */
  const string &fmt() const { return logfmt_;}
  
  /**
   * @brief: 获取LogEvent中保存的日志输入流
   * @return stringstream& 
   */  
  stringstream& SS() { return ss_; }

private:
  string logfmt_;
  stringstream ss_;
};

/**
 * @brief 日志事件Wrapper，主要使用RAII来实现在对象销毁的时候输出
*/
class LoggerWrapper {
public:
  /**
   * @brief: 构造函数
   * @param {Logger*} logger，日志器，主要用其中的stringstream
   * @param {LoggerEventPtr} loggerEventPtr
   * @return {*}
   */
  LoggerWrapper(Logger* logger, std::shared_ptr<LoggerEvent> loggerEvent);

  /**
   * @brief: 析构函数，主要用于打印日志
   * @return {*}
   */  
  ~LoggerWrapper();
  
  /**
   * @brief 返回该封装器封装Logger中的stringstream
  */
  stringstream& SS() {
    return loggerEvent_->SS();
  }

private:
  Logger *logger_;
  std::shared_ptr<LoggerEvent> loggerEvent_;
};

};

#define LOG_LEVEL(level)  if (winterfell::g_logger.GetInstance()->getLogLevel() <= level)  \
              winterfell::LoggerWrapper(winterfell::g_logger.GetInstance(), std::shared_ptr<winterfell::LoggerEvent>(new winterfell::LoggerEvent(level, time(0), winterfell::GetThreadId(), __FILE__, __LINE__))).SS()
  

/**
 * @brief 下面是日志输出
*/
#define LOG_TRACE LOG_LEVEL(winterfell::Logger::TRACE)
#define LOG_DEBUG LOG_LEVEL(winterfell::Logger::DEBUG)
#define LOG_INFO  LOG_LEVEL(winterfell::Logger::INFO)
#define LOG_WARN  LOG_LEVEL(winterfell::Logger::WARN)
#define LOG_ERROR LOG_LEVEL(winterfell::Logger::ERROR)
#define LOG_FATAL LOG_LEVEL(winterfell::Logger::FATAL)
