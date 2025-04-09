#pragma once

#include "stdafx.h"

#include "TaskQueue.h"

class TaskManager final
{
public:
	void Produce(uint8_t const targetQueueIndex, ITask const& task)
	{
		//TODO : index 검사
		auto& targetTaskQueue = _taskQueues[targetQueueIndex];

		targetTaskQueue.Enqueue(task);
	};
	void Consume(uint8_t const targetQueueIndex)
	{
		//TODO : index 검사
		auto& targetTaskQueue = _taskQueues[targetQueueIndex];

		auto task = targetTaskQueue.Dequeue();
		task.Excute();
	};
	
private:
	std::array<TaskLockQueue, 10> _taskQueues; // TODO : task 타입을 특정하자
};