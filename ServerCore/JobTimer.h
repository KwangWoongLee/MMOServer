#pragma once

struct JobData
{
	JobData(std::weak_ptr<JobQueue> owner, JobRef job) : owner(owner), job(job)
	{

	}

	weak_ptr<JobQueue>	owner;
	JobRef				job;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return executeTick > other.executeTick;
	}

	uint64_t executeTick = 0;
	JobData* jobData = nullptr;
};

/*--------------
	JobTimer
---------------*/

class JobTimer
{
public:
	void			Reserve(uint64_t tickAfter, weak_ptr<JobQueue> owner, JobRef job);
	void			Distribute(uint64_t now);
	void			Clear();

private:
	USE_LOCK;
	std::priority_queue<TimerItem>	mItems;
	std::atomic<bool>				mDistributing = false;
};

