#pragma once

#include <mutex>
#include <unordered_map>

template<typename Key, typename Value>
class ConcurrentMap {
public:
    void put(Key const &key, Value const &value) {
        m_setMutex.lock();
        m_map[key] = value;
        m_setMutex.unlock();
    }

    bool contains(Key const &key) {
        m_setMutex.lock();

        if (m_map.find(key) != m_map.end()) {
            m_setMutex.unlock();
            return true;
        }

        m_setMutex.unlock();
        return false;
    }

    Value pop(Key const &key) {
        m_setMutex.lock();
        auto result = m_map[key];
        m_map.erase(key);
        m_setMutex.unlock();
        return result;
    }

private:
    std::mutex m_setMutex;
    std::unordered_map<Key, Value> m_map;
};
