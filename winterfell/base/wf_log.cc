/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 14:47:31
 * @Description: 日志库实现
 */

#include "wf_log.h"
#include "winterfell/base/wf_timestamp.h"

#include <iostream>

#include <cstdlib> // abort
#include <unordered_map>
#include <string>
#include <cstdio>


using std::cout;
using std::endl;
using std::unordered_map;
using std::string;
namespace winterfell
{
Singleton<Logger> g_logger; 

void setLoggerLevel(Logger::LoggerLevel level) {
  g_logger.GetInstance()->setLogLevel(level);
}

const string &getLevelString(Logger::LoggerLevel level) {
  static unordered_map<int, string> mm{
      {Logger::TRACE, "TRACE"},
      {Logger::DEBUG, "DEBUG"},
      {Logger::INFO, "INFO"},
      {Logger::WARN, "WARN"},
      {Logger::ERROR, "ERROR"},
      {Logger::FATAL, "FATAL"},
  };
  return mm[level];
}

LoggerEvent::LoggerEvent(Logger::LoggerLevel level, time_t t, uint32_t thread_id,
                         const char *fileName, uint32_t lineno)
: level_(level) {
  std::stringstream ss;
  ss << getLevelString(level) << " [" << Timestamp::now().toFormattedString() << "] " << thread_id << " " << "%s" << " - " << fileName << ":" << lineno;
  logfmt_ = ss.str();
}

LoggerWrapper::LoggerWrapper(Logger *logger, std::shared_ptr<LoggerEvent> loggerEvent) 
: logger_(logger), loggerEvent_(loggerEvent) {
}

LoggerWrapper::~LoggerWrapper() {
  ::printf(string(loggerEvent_->fmt() + string("\n")).c_str(), loggerEvent_->SS().str().c_str());
  loggerEvent_->SS().clear();
  if (loggerEvent_->getLevel() == Logger::FATAL) {
    abort();
  } 
}

} // namespace winterfell
