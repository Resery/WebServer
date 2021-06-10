#include <iostream>
#include <thread>
#include <list>
#include <pthread.h>
#include "epoll.h"
#include "sync.h"

template <typename T>
class ThreadPool {
public:
    ThreadPool(unsigned int threadnum);
    ~ThreadPool();

    bool Append(T * request);

private:
    static void * Worker(void * arg);
    void Run();

private:
    // 当前线程数量
    unsigned int threadNum_;
    // 请求队列中的请求数量
    unsigned int requestNum_;

    // 最大请求数量
    const unsigned int maxRequest_ = 10000;
    // 最大线程数量
    const unsigned int maxthreadNum_ = 8; 

    bool Running_;

    // 存储线程相关信息的数组
    pthread_t * threads_;
    // 存储请求的链表也就是请求队列
    std::list<T *> request_;

    Mutex mutex_;
    Sem sem_;
};

template <typename T>
ThreadPool<T>::ThreadPool(unsigned int threadnum) {
    if (threadnum > maxthreadNum_) throw std::exception();

    threads_ = new pthread_t[threadnum];
    threadNum_ = threadnum;

    Running_ = true;

    for (unsigned int i = 0; i < threadnum; i++) {
        if (pthread_create(threads_ + i, NULL, Worker, this) != 0) {
            delete [] threads_;
            throw std::exception();
        }
        if (pthread_detach(threads_[i]) != 0) {
            delete [] threads_;
            throw std::exception();
        }
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    delete [] threads_;
    Running_ = false;
}

template <typename T>
bool ThreadPool<T>::Append(T * request) {
    mutex_.Lock();

    if (request_.size() > maxRequest_) {
        mutex_.UnLock();
        return false;
    }
    
    request_.push_back(request);
    mutex_.UnLock();
    sem_.Post();

    return true;
}

template <typename T>
void * ThreadPool<T>::Worker(void * arg) {
    ThreadPool * pool = (ThreadPool *)arg;
    pool->Run();
    return pool;
}

template <typename T>
void ThreadPool<T>::Run() {
    while (Running_) {
        sem_.Wait();
        mutex_.Lock();

        if (request_.empty()) {
            mutex_.UnLock();
            continue;
        }

        T * request = request_.front();
        request_.pop_front();
        mutex_.UnLock();

        if (!request) continue;

        request->MainStateMachine();
        // request->Process();
    }
}