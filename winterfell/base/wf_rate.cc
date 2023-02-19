/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-18 15:07:30
 * @Description: 实现限流器功能
 */

#include <winterfell/base/wf_rate.h>


using namespace winterfell;
using namespace winterfell::rate;

Limit rate::Every(Timestamp interval) {
  if (interval.microSecondsSinceEpoch() <= 0) {
    return Inf;
  }
  return 1.0 * Timestamp::MicroSecondsPerSecond() / Limit(interval.microSecondsSinceEpoch()); 
}

Timestamp rate::durationFromTokens(Limit limit, double tokens) {
  auto seconds = tokens / limit;
  return Timestamp(static_cast<int64_t>( seconds * Timestamp::MicroSecondsPerSecond() ) );
}

Limit Limiter::limit() {
  MutexLockGuard lock(mutex_);
  return limit_;
}

int Limiter::burst() {
  MutexLockGuard lock(mutex_);
  return burst_;
}

double Limiter::advance(Timestamp t) {
  auto last = last_;
  if (t.microSecondsSinceEpoch() < last.microSecondsSinceEpoch()) {
    last = t;
  }
  auto elapsed = t.sub(last);
  auto delta = limit_ * elapsed.secondsSinceEpochDouble();
  auto tokens = tokens_ + delta;
  if (tokens > burst_) {
    tokens = burst_;
  }
  return tokens;
}

double Limiter::tokensAt(Timestamp t) {
  MutexLockGuard lock(mutex_);
  return advance(t);
}

double Limiter::tokens() {
  return tokensAt(Timestamp::now());
}

Reservation Limiter::reserve() {
  return reserveN(1);
}

Reservation Limiter::reserveN(int n) {
  auto now = Timestamp::now();
  if (limit_ == Inf) {
		return Reservation(true, this, n, now);
	}
  auto tokensAtNow = tokens();
  tokensAtNow -= n;
  Timestamp waitDuring;
  if (tokensAtNow < 0) {
    waitDuring = durationFromTokens(limit_, -tokensAtNow);
  }
  bool ok = n <= burst_;
  Timestamp timeToAct;
  if (ok) {
    timeToAct = addTime(now, waitDuring.secondsSinceEpochDouble());
    this->setLast(now);
  }
  
  return Reservation(ok, this, static_cast<int>(tokensAtNow), timeToAct);
}

Timestamp Reservation::delay() {
  if (!ok_) {
    return Timestamp(INT64_MAX);
  }
  auto delay = timeToAct_.sub(Timestamp::now());
  if (delay.microSecondsSinceEpoch() < 0) {
    delay = Timestamp(0);
  }
  return delay;
}
