#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <sys/epoll.h>

class Epoll {
private:
    static const size_t MaxEvents = 1024;

    int epollFd_;
    epoll_event events_[MaxEvents];

public:
    Epoll() : epollFd_(-1) { Create(); }

    ~Epoll() { Release(); }

    bool EpollIsVaild() { return (epollFd_ >= 0); }

    bool Create() {
        if (!EpollIsVaild() || (epollFd_ = epoll_create(1)) == -1) {
            std::cerr <<  "Create Epoll Failed" << std::endl;
            return false;
        }
        return true;
    }

    bool Add(int fd, int event) {
        if (EpollIsVaild()) {
            epoll_event ev;
            ev.events = event;
            return epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) != -1;
        }
        return false;
    }

    bool Mod(int fd, int event) {
        if (EpollIsVaild()) {
            epoll_event ev;
            ev.events = event;
            return epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) != -1;
        }
        return false;
    }

    bool Del(int fd, int event) {
        if (EpollIsVaild()) {
            epoll_event ev;
            ev.events = event;
            return epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev) != -1;
        }
        return false;
    }

    void Release() {
        if (EpollIsVaild()) { close(epollFd_); }
        epollFd_ = -1;
    }

    bool Wait(int timeout) {
        if (EpollIsVaild())
            return epoll_wait(epollFd_, events_, MaxEvents, timeout);
    }

    epoll_event GetEvent(size_t index) {
        assert(index < MaxEvents);
        return events_[index];
    }
};