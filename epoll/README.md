# epoll library

This library is a simple wrapper around the basic epoll functionality.

# loading the library

```
const { epoll } = just.library('epoll')
```

this will search in the standard lib directories on your host for the shared object named epoll.so. if you want to load a specific library from a known path you can add a second parameter specifying the path:

```
const { epoll } = just.library('epoll', './epoll.so')
```

# api

## epoll.create

## epoll.control

## epoll.wait

# constants

## constants for managing the set of file descriptors being monitored by epoll 
- EPOLL_CTL_ADD
- EPOLL_CTL_MOD
- EPOLL_CTL_DEL

## constants for determining the type of event raised
EPOLLIN
EPOLLOUT
EPOLLERR
EPOLLHUP

## constants for modifying the behavior of epoll event handling
EPOLLET
EPOLLEXCLUSIVE
EPOLLONESHOT
EPOLL_CLOEXEC

# usage

# manpages

- epoll: https://man7.org/linux/man-pages/man7/epoll.7.html
- epoll_create: https://man7.org/linux/man-pages/man2/epoll_create.2.html
https://man7.org/linux/man-pages/man2/epoll_create1.2.html
https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
https://man7.org/linux/man-pages/man2/epoll_wait.2.html
