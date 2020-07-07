#include <Utility/worker_thread.h>

#include <string>
#include <iostream>

namespace poke {
void WorkerThread::StartWorker() {
    std::unique_lock<std::mutex> lock(mutex_);
    do {
        while (isRunning_ && tasks_.empty()) { itemInQueue_.wait(lock); }

        while (!tasks_.empty()) {
            const auto task = tasks_.front();
            tasks_.pop();
            lock.unlock();
            task();
            lock.lock();
        }
        itemInQueue_.notify_all();
    } while (isRunning_);
    itemInQueue_.notify_all();
}

WorkerThread::WorkerThread()
    : isRunning_(false) {
    thread_.reset(new std::thread([this] {
        isRunning_ = true;
        this->StartWorker();
    }));
}

WorkerThread::~WorkerThread() { Stop(); }

void WorkerThread::DoAsync(const std::function<void()>& task) {
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push(task);
    itemInQueue_.notify_one();
    lock.unlock();
}

void WorkerThread::DoSync(const std::function<void()>& task) {
    std::condition_variable event;
    bool finished = false;

    std::unique_lock<std::mutex> l(mutex_);
    const auto lambda = [this, &task, &finished, &event] {
        task();
        std::lock_guard<std::mutex> l(mutex_);

        finished = true;
        event.notify_one();
    };
    tasks_.push(lambda);
    itemInQueue_.notify_one();

    while (!finished)
        event.wait(l);
}

void WorkerThread::Wait() {
    std::unique_lock<std::mutex> l(mutex_);
    while (!tasks_.empty())
        itemInQueue_.wait(l);
}

void WorkerThread::Stop() {
    {
        std::lock_guard<std::mutex> l(mutex_);
        isRunning_ = false;
        itemInQueue_.notify_one();
    }
    thread_->join();
}
} //namespace poke
