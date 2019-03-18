#pragma once

#include <mutex>
#include <unordered_map>

template<typename Key, typename Value>
class ConcurrentMap {
public:
  void add(Key const &key, Value const &value) {
    m_setMutex.lock();
    m_map[key] = value;
    m_setMutex.unlock();
  }

  std::optional<Value> get(Key const &key) {
    m_setMutex.lock();

    if (m_map.find(key) != m_map.end()) {
      auto result = m_map[key];
      m_setMutex.unlock();
      return result;
    }

    m_setMutex.unlock();
    return std::optional<Value>();
  }

  void remove(Key const &key) {
    m_setMutex.lock();

    if (m_map.find(key) != m_map.end()) {
      m_map.erase(key);
    }

    m_setMutex.unlock();
  }

  bool empty() {
    m_setMutex.lock();
    bool isEmpty = m_map.empty();
    m_setMutex.unlock();
    return isEmpty;
  }

private:
  std::mutex m_setMutex;
  std::unordered_map<Key, Value> m_map;
};
