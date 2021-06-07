#include <iostream>
#include <thread>
#include <list>
#include <pthread.h>
#include <semaphore.h>

class ThreadPool {
public:
    ThreadPool(unsigned int threadnum);
    ~ThreadPool();
    
    bool Append(int * request);

private:
    static void * Worker(void * arg);
    void Run();

private:
    // 线程数量
    unsigned int threadNum_;
    // 最大请求数量
    static const unsigned int maxRequest_ = 10000;
    // 最大线程数量
    static const unsigned int maxthreadNum_ = 8;

    // 线程数组
    static pthread_t * threads_;
    // 请求队列
    static std::list<int *> requests_;

    bool Running_;

    // 线程同步所使用的管程，用来保护请求队列，因为请求队列相对于所有的对象来说是共享的
    pthread_mutex_t mutex_;
    // 线程同步所使用的信号量，用来通知当前是否有进程需要被处理
    sem_t sem_;
};