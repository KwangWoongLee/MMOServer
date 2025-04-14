#pragma once

#include "stdafx.h"
#include "Task.h"

enum class ETaskProduceType : uint8_t
{
	BASIC,
	MAX
};

size_t constexpr TaskProduceTypeMax = static_cast<size_t>(ETaskProduceType::MAX);

namespace
{
	struct TaskBufferSet
	{
		LockQueue<ITask> _tasks;
		std::queue<ITask> _tasksBackBuffer;
	};

}

class TaskManager final
{
public:
    using Singleton = Singleton<TaskManager>;

public:
	void Produce(ETaskProduceType const taskProduceType, ITask const& task)
	{
		auto const producerIndex = static_cast<size_t>(taskProduceType);
		//TODO : index 검사
		
		auto& tasks = _taskBufferSetsByProduceType.at(producerIndex)._tasks;
		tasks.Enqueue(task);
	}

	void Consume(ETaskProduceType const taskProduceType)
	{
		auto const producerIndex = static_cast<size_t>(taskProduceType);
		//TODO : index 검사

		auto& tasks = _taskBufferSetsByProduceType.at(producerIndex)._tasks;
		auto& tasksBackBuffer = _taskBufferSetsByProduceType.at(producerIndex)._tasksBackBuffer;
		tasks.DequeueBySwap(tasksBackBuffer);

        while (not tasksBackBuffer.empty())
        {
			auto& task = tasksBackBuffer.front();
            task.Excute();
			tasksBackBuffer.pop();
        }
	}
	
private:
	std::array<TaskBufferSet, TaskProduceTypeMax> _taskBufferSetsByProduceType;
};