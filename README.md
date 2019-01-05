# Linux下特有定时器

该例子封装一个 timerfd 定时器, main 中创建实例并使用 epoll 监听
## 相对时间

`CLOCK_REALTIME`
可设定的系统级实时时钟.
相对时间，从1970.1.1到目前的时间。更改系统时间会更改获取的值。也就是说，它以系统时间为坐标。

## 绝对时间

`CLOCK_MONOTONIC`
不可设定的恒定态时钟
与`CLOCK_REALTIME`相反，它是以绝对时间为准，获取的时间为系统重启到现在的时间，更改系统时间对齐没有影响。

使用完定时器记得`::close`

---


# 注意!

默认情况下，`new_value`中指定的初始过期时间相对于调用时计时器时钟上的当前时间(即, `new_value.it_value`指定相对于 clockid 指定的时钟的当前值的时间)。

所以使用`timerfd_settime`设置初始到期时间时, 在`new_value.it_value`中设置初次定时的时间

如果`timerfd_settime`第二个参数设置为 0, `new_value.it_value`设置为 `firsttime`
如果`timerfd_settime`第二个参数设置为`TFD_TIMER_ABSTIME`, `new_value.it_value`设置为 `now.tv_sec + firsttime`
