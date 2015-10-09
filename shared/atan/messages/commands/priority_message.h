#pragma once

template<typename T>
struct priority_message {
    T message;
    int priority;

    priority_message(T&& message, int priority) : message(std::forward<T>(message)), priority(priority) { }
    priority_message(T& message, int priority) : message(std::move(message)), priority(priority) { }

    priority_message(priority_message<T>& p_msg) = delete;

    priority_message(priority_message<T>&& p_msg) {
        this->message = std::move(p_msg.message);
        this->priority = p_msg.priority;
    }

    priority_message& operator=(priority_message<T>& p_msg) = delete;

    priority_message& operator=(priority_message<T>&& p_msg) {
        this->message = std::move(p_msg.message);
        this->priority = p_msg.priority;
    }
};