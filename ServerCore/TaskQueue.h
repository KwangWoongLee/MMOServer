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

	ITask&& Dequeue()
	{
		//TODO : atomic<bool> isEmpty 최적화 ?


		std::scoped_lock lock(_taskMutex);

		//TODO : 하나씩 빼는게 나을지..?
		auto task = _tasks.front(); // TODO: 복사 ?
		_tasks.pop();

		return std::move(task);
	};

private:
	std::mutex _taskMutex;
	std::queue<ITask> _tasks;
};