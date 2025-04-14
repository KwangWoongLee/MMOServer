#pragma once

#include "stdafx.h"

template <typename T>
class LockQueue final
{
public:
	void Enqueue(T const& value)
	{
		std::scoped_lock lock(_mutex);

		_elements.emplace(value);
	};

	void Enqueue(T&& value)
	{
		std::scoped_lock lock(_mutex);

		_elements.emplace(std::move(value));
	};

	bool DequeueBySwap(std::queue<T>& swapTargetQueue)
	{
		//TODO : atomic<bool> isEmpty 최적화 ?


		std::scoped_lock lock(_mutex);

		_elements.swap(swapTargetQueue);
	}

private:
	std::mutex _mutex;
	std::queue<T> _elements;
};