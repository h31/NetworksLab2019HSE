#pragma once

#include <mutex>
#include <queue>

template<typename T>
class ConcurrentQueue {
public:
    void push(T const &response) {
        m_queueMutex.lock();
        m_resultsQueue.push(response);
        m_queueMutex.unlock();
    }

    T pop() {
        m_queueMutex.lock();
        auto result = m_resultsQueue.front();
        m_resultsQueue.pop();
        m_queueMutex.unlock();
        return result;
    }

    bool empty() {
        m_queueMutex.lock();
        bool isEmpty = m_resultsQueue.empty();
        m_queueMutex.unlock();
        return !isEmpty;
    }

private:
    std::mutex m_queueMutex;
    std::queue<T> m_resultsQueue;
};
