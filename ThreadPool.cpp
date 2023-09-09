//
// Created by zhouqi on 2023/9/6.
//

#include "ThreadPool.h"

using namespace std;
ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        threads_.emplace_back([this] {
            while (!stop) {
                function<void()> task;
                {
                    unique_lock<mutex> lock(mutex_);
                    condition_.wait(lock, [this] { return stop || !tasks_.empty(); });
                    if (stop && tasks_.empty()) {
                        return;
                    }
                    task = move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}

void ThreadPool::stop_threads() {
    stop = true;
    condition_.notify_all();
    for (auto& thread : threads_) {
        thread.join();
    }
}

ThreadPool::~ThreadPool() {
    stop_threads();
}

bool ThreadPool::gotWorkToDo() {
    unique_lock<mutex> lock(mutex_);
    return !tasks_.empty();
}

size_t ThreadPool::getJobSize() {
    return tasks_.size();
}
