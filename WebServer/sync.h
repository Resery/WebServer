#include <pthread.h>
#include <semaphore.h>
#include <exception>

class Sem {
private:
    sem_t sem_;

public:
    Sem() {
        if (sem_init(&sem_, 0, 0) == -1) {
            throw std::exception();
        }
    }

    Sem(int num) {
        if (sem_init(&sem_, 0, num) == -1) {
            throw std::exception();
        }
    };

    ~Sem() {
        sem_destroy(&sem_);
    }

    bool Wait() { return sem_wait(&sem_) == 0; }

    bool Post() { return sem_post(&sem_) == 0; }
};

class Mutex {
public:
    Mutex() {
        if (pthread_mutex_init(&mutex_, NULL) == -1) {
            throw std::exception();
        }
    };

    ~Mutex() {
        pthread_mutex_destroy(&mutex_);
    }

    void Lock() { pthread_mutex_lock(&mutex_); }

    void UnLock() { pthread_mutex_unlock(&mutex_); }

    pthread_mutex_t * Get() { return &mutex_; }

private:
    pthread_mutex_t mutex_;
};

class Condicate {
public:
    Condicate() {
        if (pthread_cond_init(&cond_, NULL) == -1) {
            throw std::exception();
        }
    }

    ~Condicate() {
        pthread_cond_destroy(&cond_);
    }

    bool Signal() { return pthread_cond_signal(&cond_) == 0; }

    bool Wait(pthread_mutex_t mutex) { return pthread_cond_wait(&cond_, &mutex) == 0;}

    bool TimeWait(pthread_mutex_t mutex, struct timespec time) {
        return pthread_cond_timedwait(&cond_, &mutex, &time) == 0;
    }

    bool broadcast() { return pthread_cond_broadcast(&cond_) == 0; }

private:
    pthread_cond_t cond_;
};