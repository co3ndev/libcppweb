#include "../../include/cppweb/threading/thread_pool.hpp"
#include <iostream>

namespace cppweb {
namespace threading {

ThreadPool::ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] { worker_thread(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, [this] { return !task_queue.empty() || stop; });

            if (stop && task_queue.empty()) {
                break;
            }

            if (!task_queue.empty()) {
                task = std::move(task_queue.front());
                task_queue.pop();
            }
        }

        if (task) {
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "[ThreadPool] Uncaught exception in task: " << e.what() << "\n";
            } catch (...) {
                std::cerr << "[ThreadPool] Unknown uncaught exception in task.\n";
            }
        }
    }
}

} // namespace threading
} // namespace cppweb
