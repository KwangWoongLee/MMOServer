#pragma once
#include "stdafx.h"

#include "Task.h"

class ITask;

class KeySerialTaskExecutor final
{
public:
    void Init(uint8_t const threadCount)
    {
        _threadCount = threadCount;
        _queuesByThread.resize(_threadCount);
    }

    void Reserve(std::shared_ptr<ITask> const& task)
    {
        std::assert(0 != _threadCount); // 명시적으로 초기화 하지 않으면 안됩니다.

        int64_t const key = task->Key();

        if (key < 0 || static_cast<size_t>(key) >= MaxKeyCount)
        {
            //TODO: log
            return;
        }

        auto const targetThreadIndex = key % _threadCount;
        auto& queue = _queuesByThread.at(targetThreadIndex);
        queue.Enqueue(task);
    }

private:
    size_t _threadCount{};
    std::vector<LockQueue<ITask>> _queuesByThread;
};
