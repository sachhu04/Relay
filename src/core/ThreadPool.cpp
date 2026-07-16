#include "ThreadPool.hpp"
#include <spdlog/spdlog.h>

namespace relay::core {

ThreadPool::ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&ThreadPool::worker_loop, this);
    }
    spdlog::info("ThreadPool initialized with {} workers", num_threads);
}

ThreadPool::~ThreadPool() {
    queue_.shutdown();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    spdlog::info("ThreadPool shutdown complete");
}

void ThreadPool::enqueue(int client_fd) {
    queue_.push(client_fd);
}

void ThreadPool::set_handler(std::function<void(int)> handler) {
    handler_ = std::move(handler);
}

void ThreadPool::worker_loop() {
    while (true) {
        auto client_fd = queue_.pop();
        if (!client_fd) {
            break; // Shutdown signal
        }
        
        if (handler_) {
            handler_(*client_fd);
        } else {
            spdlog::warn("ThreadPool has no handler set!");
        }
    }
}

} // namespace relay::core
