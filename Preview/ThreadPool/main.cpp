#include "threadpool.h"

int main() {
    ThreadPool * pool = NULL;

    try {
        pool = new ThreadPool(8);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    for (int i = 0; i < 8; i++) {
        int * tmp = &i; 
        pool->Append(tmp);
    }

    delete pool;
}
