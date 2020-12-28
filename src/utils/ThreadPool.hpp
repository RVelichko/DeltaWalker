/*!
 * \brief  Класс, реализующий управление пулом потоков.
 * \author Величко Ростислав rnvelich@mts.ru
 * \date   28.10.2020
 */

#pragma once


#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>


namespace utils {

typedef std::lock_guard<std::mutex> LockQuard;
typedef std::unique_lock<std::mutex> UniqueLock;


class ThreadPool {
    typedef std::function<void()> Task;

    std::vector<std::thread> workers_;
    std::queue<Task> qtasks_;

    std::mutex queue_mutex_;
    std::condition_variable condition_exe_;
    std::condition_variable condition_add_;
    std::atomic_bool is_run_;

    std::uint64_t threads_ = 1;

public:
    template <class Fn, class... Args>
    void enqueue(Fn &&f, Args &&... args) {
        if (is_run_) {
            std::function<void()> task(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...));
            {
                LockQuard l(queue_mutex_);
                qtasks_.push([task]() { task(); });
            }
            condition_exe_.notify_one();
            {
                UniqueLock l(queue_mutex_);
                if (!qtasks_.empty()) {
                    condition_add_.wait(l);
                }
            }
        }
    }

    ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();
};
}  // namespace utils
