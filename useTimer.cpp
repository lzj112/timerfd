#include <unistd.h>
#include <assert.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

#include <iostream>

#include "Timer.h"

using namespace std;

const int MAXNUM = 20;

int main(int argc, char *argv[])
{
    Timer timeFd;

    int epollfd = epoll_create(1);  //创建epoll实例对象

    struct epoll_event ev;
    struct epoll_event events[MAXNUM];
    ev.data.fd = timeFd.fd();
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, timeFd.fd(), &ev); //添加到epoll事件集合

    uint64_t exp;
    ssize_t s;

    timeFd.setUpTimer(2, 1);
    for (; ;) 
    {
        int num = epoll_wait(epollfd, events, MAXNUM, 0);
        assert(num >= 0);
        
        for (int i = 0; i < num; i++) 
        {
            if (events[i].events & EPOLLIN) 
            {
                //....处理其他事件
                if (events[i].data.fd == timeFd.fd()) 
                {
                    s = read(events[i].data.fd, &exp, sizeof(uint64_t)); //需要读出uint64_t大小, 不然会发生错误
                    assert(s == sizeof(uint64_t));
                    cout << "here is timer" << endl;
                }
            }
        }
    }

    close(epollfd);

    return 0;
}