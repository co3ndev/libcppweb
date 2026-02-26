#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>

namespace cppweb {
namespace threading {
    class ThreadPool {
    public:
        explicit ThreadPool(size_t num_threads = 4);

        ~ThreadPool();

        template<typename F>
        void enqueue(F&& func) {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                if (stop) {
                    throw std::runtime_error("Cannot enqueue task on stopped thread pool");
                }
                task_queue.emplace(std::forward<F>(func));
            }
            condition.notify_one();
        }

        size_t get_thread_count() const { return workers.size(); }

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> task_queue;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop = false;

        void worker_thread();
    };

} // namespace threading
} // namespace cppweb
