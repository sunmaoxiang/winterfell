/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 20:02:17
 * @Description: 声明事件戳类，用于计时 以及 获取当前时间
 */

#pragma once

#include <string>

namespace winterfell {

using std::string;

class Timestamp {
public:
  Timestamp()
    : microSecondsSinceEpoch_(0)
  {  
  }
  explicit Timestamp(int64_t microSecondsSinceEpochArg) 
    : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
  {
  }
  Timestamp(const Timestamp &) = default; 

  /**
   * @brief: 返回时间戳的字符串格式
   * @return string 
   */  
  string toString() const;

  /**
   * @brief: 以日期形式返回时间
   * @return string
   */  
  string toFormattedString(bool showMiroseconds = true) const;

  /**
   * @brief 判断时间戳是否可用
  */
  bool valid() const { return microSecondsSinceEpoch_ > 0; }

  int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
  time_t secondsSinceEpoch() const{
    return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  }
  bool invalid() { return microSecondsSinceEpoch_ < 0; }

  /**
   * @brief 获取当前时间，设置成静态函数是为了Timestamp::func()直接调用
  */
  static Timestamp now(); 
  
  static Timestamp fromUnixTime(time_t t, int microseconds) {
    return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
  }

  static Timestamp fromUnixTime(time_t t) {
    return fromUnixTime(t, 0);
  }

  bool operator < (const Timestamp& rhs) {
    return this->microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
  }
  bool operator==(const Timestamp &rhs) {
    return this->microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
  }

private:
  int64_t microSecondsSinceEpoch_;
  static const int kMicroSecondsPerSecond = 1000 * 1000; // 微秒转化成秒需要乘多少
};
};