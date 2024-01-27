//
// Created by NIgel work on 25/01/2024.
//

#ifndef REALTIME_THREADPOOL_H
#define REALTIME_THREADPOOL_H

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i){
            workers.emplace_back([this]{
                for(;;){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        condition.wait(lock, [this]{return stop || !this->tasks.empty();});
                        if (stop && tasks.empty()){
                            return;
                        }
                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                }
            });
        }
    }
    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker: workers){
            worker.join();
        }
    }

    template<class F, class... Args>
            auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>{
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop){
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace([task](){(*task)();});
        }
        condition.notify_one();
        return res;

    }

private:

    // need to keep track of threads, so we can join them
    std::vector<std::thread> workers;

    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

};


#endif //REALTIME_THREADPOOL_H
