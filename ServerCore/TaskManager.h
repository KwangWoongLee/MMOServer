#pragma once

#include "stdafx.h"

#include "TaskQueue.h"

class TaskManager final
{
public:
    using Singleton = Singleton<TaskManager>;

public:
	void Produce(uint8_t const targetQueueIndex, ITask const& task)
	{
		//TODO : index 검사
		auto& targetTaskQueue = _taskQueuesByProduceType[targetQueueIndex];

		targetTaskQueue.Enqueue(task);
	}

	void Consume(uint8_t const targetQueueIndex)
	{
		//TODO : index 검사
		auto& targetTaskQueue = _taskQueuesByProduceType[targetQueueIndex];

		auto& taskQueueBackBuffer = _taskQueueBackBuffersByProduce[targetQueueIndex];
		targetTaskQueue.DequeueBySwap(taskQueueBackBuffer);

        while (not taskQueueBackBuffer.empty())
        {
			auto const& task = taskQueueBackBuffer.front();
            task.Excute();
            taskQueueBackBuffer.pop();
        }
	}
	
private:
	std::array<TaskLockQueue, 10> _taskQueuesByProduceType; // TODO : task 타입을 특정하자
    std::array<std::queue<ITask>, 10> _taskQueueBackBuffersByProduce; // TODO : task 타입을 특정하자
};