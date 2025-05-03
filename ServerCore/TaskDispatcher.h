#pragma once

#include "stdafx.h"

#include "KeySerialTaskExecutor.h"
#include "Task.h"

class TaskDispatcher final
{
public:
    using Singleton = Singleton<TaskDispatcher>;

public:
    void Init()
    {
        for ()
    }

    void Dispatch(std::shared_ptr<ITask> const& task)
    {
        if ()

        size_t const index = static_cast<size_t>(task->GetTaskType());
        if (index >= static_cast<size_t>(ETaskType::MAX))
        {
            return;
        }

        _executors[index].Reserve(task);
    }

private:
    std::array<KeySerialTaskExecutor, TaskTypeMax> _executors;
};