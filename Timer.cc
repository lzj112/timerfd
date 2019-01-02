#include "Timer.h"

Timer::Timer(int type) : type_(type), timerfd_(-1) 
{
    timerfd_ = timerfd_create(type, TFD_NONBLOCK);
    assert(timerfd_ != -1);
}

void Timer::setUpTimer(int firstTime, int interval) 
{
    struct itimerspec new_value;
    struct timespec now;

    //获取时间
    int ret;
    ret = clock_gettime(type_, &now);
    assert(ret != -1);

    //设置到期时间
    new_value.it_value.tv_sec = now.tv_sec + firstTime; //第一次到期的时间
    new_value.it_value.tv_nsec = now.tv_nsec; 

    new_value.it_interval.tv_sec = interval;      //之后每次到期的时间间隔
    new_value.it_interval.tv_nsec = 0;

    startTimer(new_value);
}

void Timer::startTimer(const itimerspec& new_value) 
{
    int ret = timerfd_settime(timerfd_, TFD_TIMER_ABSTIME, &new_value, nullptr);
    assert(ret != -1);
}

void Timer::shutdown() 
{
    itimerspec it = {0};
    int ret = timerfd_settime(timerfd_, type_, &it, nullptr);
    assert(ret != -1);
}

int Timer::fd() 
{
    return timerfd_;
}