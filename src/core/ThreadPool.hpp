#pragma once
#include "WorkQueue.hpp"
#include <vector>
#include <thread>
#include <functional>

namespace relay::core {

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();
    
    // Disallow copy/move
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Enqueue a task (socket FD)
    void enqueue(int client_fd);

    // Register the function that will process the sockets
    void set_handler(std::function<void(int)> handler);

private:
    void worker_loop();

    std::vector<std::thread> workers_;
    WorkQueue<int> queue_;
    std::function<void(int)> handler_;
};

} // namespace relay::core
