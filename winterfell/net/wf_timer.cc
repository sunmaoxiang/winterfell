/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-10 15:13:31
 * @Description: 定时器类定义
 */

#include "wf_timer.h"
#include "winterfell/util/wf_callbacks.h"
namespace winterfell {

Timer::Timer(Timestamp whenArg, double intervalArg, TimerCallback& tcb) 
: when_(whenArg),
  repeaded_( intervalArg > 0.0 ),
  interval_(intervalArg),
  tcb_(tcb)
{  
}

void Timer::runTcb() {
  this->tcb_();
}

void Timer::updateWhen() {
  when_ = addTime( Timestamp::now(), this->interval_ );
}

}