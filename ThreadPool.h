//
// Created by zhouqi on 2023/9/6.
//

#ifndef CPPCRAWLER_THREADPOOL_H
#define CPPCRAWLER_THREADPOOL_H
#include <thread>
#include <chrono>
#include <future>
#include <queue>
#include <mutex>


class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (stop) {
                throw std::runtime_error("ThreadPool is stopped");
            }
            tasks_.emplace([task] { (*task)(); });
        }
        condition_.notify_one();
        return res;
    }
    bool gotWorkToDo();
    size_t getJobSize();
    void stop_threads();

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stop;
};


#endif //CPPCRAWLER_THREADPOOL_H
