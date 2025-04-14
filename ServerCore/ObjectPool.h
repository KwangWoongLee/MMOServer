#pragma once
#include "stdafx.h"

template <typename T>
class ObjectPool
{
public:
    using SPtr = std::shared_ptr<T>;
    using Singleton = Singleton<ObjectPool<T>>;

public:
    explicit ObjectPool(size_t const initialCapacity = 0)
    {
        for (size_t i = 0; i < initialCapacity; ++i)
        {
            _pool.emplace_back(std::make_shared<T>());
        }
    }

    SPtr Acquire()
    {
        std::scoped_lock lock(_mutex);

        if (!_pool.empty())
        {
            SPtr obj = _pool.back();
            _pool.pop_back();
            return obj;
        }
        else
        {
            return createNew();
        }
    }

    void Release(SPtr const& obj)
    {
        std::scoped_lock lock(_mutex);
        _pool.emplace_back(obj);
    }

private:
    SPtr createNew() const
    {
        return std::make_shared<T>();
    }

private:
    std::deque<SPtr> _pool;
    std::mutex _mutex;
};
