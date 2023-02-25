#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(Callback&& callback)
	{
		Push(std::make_shared<Job>(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(std::make_shared<Job>(owner, memFunc, std::forward<Args>(args)...));
	}

	void DoTimer(uint64 tick, std::function<void()>&& callback)
	{
		JobRef job = std::make_shared<Job>(std::move(callback));
		gJobTimer->Reserve(tick, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		JobRef job = std::make_shared<Job>(owner, memFunc, std::forward<Args>(args)...);
		gJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}


	void					Push(JobRef job, bool pushOnly = false);
	void					Execute();
	void					Clear() { mJobQueue.Clear(); }


protected:
	LockQueue<JobRef>		mJobQueue;
	std::atomic<int32>		mJobCount = 0;

};

