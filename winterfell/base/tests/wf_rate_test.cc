/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-19 11:39:17
 * @Description: 测试rate包
 */

#include <winterfell/base/wf_rate.h>
#include <winterfell/base/wf_log.h>
#include <unistd.h>
#include <cassert>
using namespace winterfell;

int main() {
  auto limit = rate::Every(Timestamp(static_cast<int64_t>(0.1 *  Timestamp::MicroSecondsPerSecond())));
  LOG_INFO << limit;

  rate::Limiter limiter(limit, 2);
  
  auto reserv = limiter.reserveN(3);
  assert(!reserv.OK());

  reserv = limiter.reserve();
  assert(reserv.OK());
  assert(reserv.delay().secondsSinceEpoch() == 0 );
  LOG_INFO << reserv.delay().microSecondsSinceEpoch();

  reserv = limiter.reserveN(2);
  LOG_INFO <<reserv.delay().microSecondsSinceEpoch() ;
  usleep(static_cast<__useconds_t>( reserv.delay().microSecondsSinceEpoch() ));
  reserv = limiter.reserveN(2);
  LOG_INFO << reserv.delay().microSecondsSinceEpoch();

  return 0;
}
