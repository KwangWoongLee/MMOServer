#pragma once

#include "stdafx.h"

#include "Task.h"

class TaskLockQueue final
{
public:
	void Enqueue(ITask const& task)
	{
		std::scoped_lock lock(_taskMutex);

		_tasks.emplace(task);
	};

	ITask&& DequeueBySwap(std::queue<ITask>& swapTargetQueue)
	{
		//TODO : atomic<bool> isEmpty 최적화 ?


		std::scoped_lock lock(_taskMutex);

        _tasks.swap(swapTargetQueue);
	}

private:
	std::mutex _taskMutex;
	std::queue<ITask> _tasks;
};