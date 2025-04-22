#pragma once
#include "stdafx.h"

template <typename T>
class ObjectPool
{
public:
    using Singleton = Singleton<ObjectPool<T>>;

public:
    explicit ObjectPool(size_t const initialCapacity = 0)
    {
        for (size_t i = 0; i < initialCapacity; ++i)
        {
            _pool.emplace_back(new T());
        }
    }

    T* Acquire()
    {
        std::scoped_lock lock(_mutex);

        if (!_pool.empty())
        {
            auto* obj = _pool.back();
            _pool.pop_back();
            return obj;
        }
        else
        {
            return new T();
        }
    }

    void Release(T const* obj)
    {
        std::scoped_lock lock(_mutex);
        _pool.emplace_back(obj);
    }

private:
    std::deque<T*> _pool;
    std::mutex _mutex;
};
