#include "threadpool.h"

ThreadPool::ThreadPool(unsigned int threadnum) {
    if (threadnum > maxthreadNum_) throw std::exception();

    int num = 0;

    threads_ = new pthread_t[threadnum];

    sem_init(&sem_, 0, 0);
    pthread_mutex_init(&mutex_, NULL);

    Running_ = true;

    for (num = 0; num < threadnum; num++) {
        if (pthread_create(threads_ + num, NULL, Worker, this) != 0) {
            delete [] threads_;
            throw std::exception();
        }

        if (pthread_detach(threads_[num]) != 0) {
            delete [] threads_;
            throw std::exception();
        }
    }

    threadNum_ = threadnum;
}

ThreadPool::~ThreadPool() {
    delete [] threads_;
    sem_destroy(&sem_);
    pthread_mutex_destroy(&mutex_);
    Running_ = false;
}

bool ThreadPool::Append(int * request) {
    pthread_mutex_lock(&mutex_);

    if (requests_.size() > maxRequest_) {
        pthread_mutex_unlock(&mutex_);
        throw std::exception();
    }

    requests_.push_back(request);
    pthread_mutex_unlock(&mutex_);
    sem_post(&sem_);

    return true;
}

void * ThreadPool::Worker(void * argThis) {
    ThreadPool * pool = (ThreadPool *)argThis;
    pool->Run();
    return pool;
}

void ThreadPool::Run() {
    while (Running_) {
        sem_wait(&sem_);
        pthread_mutex_lock(&mutex_);

        if (requests_.empty()) {
            pthread_mutex_unlock(&mutex_);
            continue;
        }

        int * request = requests_.front();

        std::clog << *request << std::endl;

        requests_.pop_front();
        pthread_mutex_unlock(&mutex_);

        if (!request) continue;
    }
}