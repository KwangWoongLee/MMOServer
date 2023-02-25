#include "stdafx.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(JobRef job, bool pushOnly)
{
	const int32 prevCount = mJobCount.fetch_add(1);
	mJobQueue.Push(job); // WRITE_LOCK

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (TLS_JobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			gGlobalQueue->Push(shared_from_this());
		}
	}
}

// 1) 일감이 너~무 몰리면?
void JobQueue::Execute()
{
	TLS_JobQueue = this;

	while (true)
	{
		std::vector<JobRef> jobs;
		mJobQueue.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		// 남은 일감이 0개라면 종료
		if (mJobCount.fetch_sub(jobCount) == jobCount)
		{
			TLS_JobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= TLS_LastTickCount)
		{
			TLS_JobQueue = nullptr;
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			gGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}
