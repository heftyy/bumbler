#pragma once

#include <thread>
#include <atomic>

namespace bumbler {

class scheduler;

class cancellable {
public:
    friend class scheduler;

    std::atomic<std::thread::id> thread_id;

    cancellable() : cancel_(false), cancelled_(false) { }

    cancellable(std::thread::id thread_id) : thread_id(thread_id), cancel_(false), cancelled_(false) { }

    ~cancellable() {
        cancel_ = true;
        cancelled_ = true;
    }

    void cancel() {
        cancel_ = true;
    }

    bool is_cancelled() {
        return cancelled_;
    }

protected:
    bool check_cancel() {
        return cancel_;
    }

    void cancelled() {
        cancelled_ = true;
    }

private:
    std::atomic<bool> cancel_;
    std::atomic<bool> cancelled_;
};

}
