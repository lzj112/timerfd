#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>  // Definition of uint64_t

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

static void
print_elapsed_time(void) // 用于输出消逝的时间
{
    static struct timespec start; // 开始的时间
    struct timespec curr; // 现在的时间
    static int first_call = 1; // first_call是一个静态变量,只会初始化一次
    int secs, nsecs;

    if (first_call) { 
        first_call = 0; // 也就是说,仅第1次调用print_elapsed_time函数才会运行到这里
        // 并且记录下第1次调用该函数时的时间,以后再次调用该函数,就可以获得差值了.
        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
            handle_error("clock_gettime");
    }

    if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
        handle_error("clock_gettime");

    secs = curr.tv_sec - start.tv_sec;
    nsecs = curr.tv_nsec - start.tv_nsec; // 得到curr和start之间的差值
    if (nsecs < 0) {
        secs--;
        nsecs += 1000000000;
    }
    printf("%d.%03d: ", secs, (nsecs + 500000) / 1000000);
}

int
main(int argc, char *argv[])
{
    struct itimerspec new_value;
    int max_exp = 100;
    int fd;
    struct timespec now;
    uint64_t exp, tot_exp;
    ssize_t s;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1) // 得到现在的时间
        handle_error("clock_gettime");

    // Create a CLOCK_REALTIME absolute timer with initial
    // expiration and interval as specified in command line
    //printf("%s %s %s\n", argv[1], argv[2], argv[3]);
    new_value.it_value.tv_sec = now.tv_sec + 2; // it_value指的是第一次到期的时间
    new_value.it_value.tv_nsec = now.tv_nsec; 

    new_value.it_interval.tv_sec = 1;  
    new_value.it_interval.tv_nsec = 0;

    fd = timerfd_create(CLOCK_REALTIME, 0); // 构建了一个定时器
    if (fd == -1)
        handle_error("timerfd_create");

    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
        handle_error("timerfd_settime");

    print_elapsed_time();
    printf("timer started\n"); // 定时器开启啦！

    for (tot_exp = 0; tot_exp < max_exp;) { // max_exp原来是次数，是吧！
        s = read(fd, &exp, sizeof(uint64_t)); // 也就是说，如果fd不是非阻塞的,那么程序会阻塞在这里
        if (s != sizeof(uint64_t))
            handle_error("read");

        tot_exp += exp;
        print_elapsed_time();
        printf("read: %llu; total=%llu\n",
            (unsigned long long) exp,
            (unsigned long long) tot_exp);
    }

    exit(EXIT_SUCCESS);
}
