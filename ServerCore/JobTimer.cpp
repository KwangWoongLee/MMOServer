#include "stdafx.h"
#include "JobQueue.h"

void JobTimer::Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;

	mItems.push(TimerItem{ executeTick, jobData });
}

void JobTimer::Distribute(uint64 now)
{
	// �� ���� 1 �����常 ���
	if (mDistributing.exchange(true) == true)
		return;

	std::vector<TimerItem> items;

	{
		WRITE_LOCK;

		while (mItems.empty() == false)
		{
			const TimerItem& timerItem = mItems.top();
			if (now < timerItem.executeTick)
				break;

			items.push_back(timerItem);
			mItems.pop();
		}
	}

	for (TimerItem& item : items)
	{
		if (JobQueueRef owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);

		ObjectPool<JobData>::Push(item.jobData);
	}

	// �������� Ǯ���ش�
	mDistributing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;

	while (mItems.empty() == false)
	{
		const TimerItem& timerItem = mItems.top();
		ObjectPool<JobData>::Push(timerItem.jobData);

		mItems.pop();
	}
}
