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
    Epoll(int epollfd) : epollFd_(epollfd) {}

    ~Epoll() { Release(); }

    int GetEpollFd() { return epollFd_; }

    bool EpollIsVaild() { return (epollFd_ >= 0); }

    bool Create() {
        if ((epollFd_ = epoll_create(1)) == -1 || !EpollIsVaild()) {
            std::cerr <<  "Create Epoll Failed" << std::endl;
            return false;
        }

        memset(events_, 0, sizeof(events_));

        return true;
    }

    bool Add(int fd, int event) {
        if (EpollIsVaild()) {
            epoll_event ev;
            ev.events = event;
            ev.data.ptr = 0;
            ev.data.fd = fd;
            return epoll_ctl(epollFd_, EPOLL_CTL_ADD, ev.data.fd, &ev) != -1;
        }
        return false;
    }

    bool Mod(int fd, int event) {
        if (EpollIsVaild()) {
            epoll_event ev;
            ev.events = event;
            ev.data.ptr = 0;
            ev.data.fd = fd;
            return epoll_ctl(epollFd_, EPOLL_CTL_MOD, ev.data.fd, &ev) != -1;
        }
        return false;
    }

    bool Del(int fd, int event) {
        if (EpollIsVaild()) {
            epoll_event ev;
            ev.events = event;
            ev.data.ptr = 0;
            ev.data.fd = fd;
            return epoll_ctl(epollFd_, EPOLL_CTL_DEL, ev.data.fd, &ev) != -1;
        }
        return false;
    }

    void Release() {
        if (EpollIsVaild()) { close(epollFd_); }
        epollFd_ = -1;
    }

    int Wait(int timeout) {
        if (EpollIsVaild()) {
            // memset(events_, 0, sizeof(events_));
            return epoll_wait(epollFd_, events_, MaxEvents, timeout);
        }

        return false;
    }

    epoll_event GetEvent(size_t index) {
        assert(index < MaxEvents);
        return events_[index];
    }
};