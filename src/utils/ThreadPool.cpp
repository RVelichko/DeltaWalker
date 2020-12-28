#include <sstream>

#include "ThreadPool.hpp"

using namespace utils;


ThreadPool::ThreadPool(size_t threads) : is_run_(true), threads_(threads ? threads : 1) {
    for (std::size_t i = 0; i < threads_; ++i) {
        workers_.emplace_back([this] {
            while (true) {
                Task task;
                {
                    UniqueLock l(queue_mutex_);
                    while (is_run_ && qtasks_.empty()) {
                        condition_add_.notify_one();
                        condition_exe_.wait(l);
                    }
                    if (!is_run_ && qtasks_.empty()) {
                        return;
                    }
                    task = qtasks_.front();
                    qtasks_.pop();
                }
                task();
                condition_add_.notify_one();
            }
        });
    }
}


ThreadPool::~ThreadPool() {
    is_run_ = false;
    condition_add_.notify_one();
    condition_exe_.notify_all();
    for (auto &worker : workers_) {
        worker.join();
    }
}
