/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 14:47:31
 * @Description: 日志库实现
 */

#include "wf_log.h"

#include <iostream>

#include <cstdlib> // abort
#include <unordered_map>
#include <string>

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
  static unordered_map<int, string> mm {
      {Logger::DEBUG, "DEBUG"},
      {Logger::INFO, "INFO"},
      {Logger::WARN, "WARN"},
      {Logger::ERROR, "ERROR"},
      {Logger::FATAL, "FATAL"},
  };
  return mm[level];
}

void Logger::log(const LoggerEvent &loggerEvent) {
  // FIXME: 还没实现同步
  cout << loggerEvent.fmt() << endl;
}

LoggerEvent::LoggerEvent(Logger::LoggerLevel level, time_t t, uint32_t thread_id,
                     const string &msg, const char *fileName, uint32_t lineno) {
  std::stringstream ss;
  string dateString(asctime(gmtime(&t)));
  dateString = dateString.substr(0, dateString.size() - 1);
  ss << getLevelString(level) << " [" << dateString << "] " << thread_id << " " << msg << " - " << fileName << ":" << lineno;
  logfmt_ = ss.str();
}

} // namespace winterfell
