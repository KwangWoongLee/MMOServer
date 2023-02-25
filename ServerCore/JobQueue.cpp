#include "stdafx.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(JobRef job, bool pushOnly)
{
	const int32 prevCount = mJobCount.fetch_add(1);
	mJobQueue.Push(job); // WRITE_LOCK

	// ù��° Job�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
		// �̹� �������� JobQueue�� ������ ����
		if (TLS_JobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue�� �ѱ��
			gGlobalQueue->Push(shared_from_this());
		}
	}
}

// 1) �ϰ��� ��~�� ������?
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

		// ���� �ϰ��� 0����� ����
		if (mJobCount.fetch_sub(jobCount) == jobCount)
		{
			TLS_JobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= TLS_LastTickCount)
		{
			TLS_JobQueue = nullptr;
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue�� �ѱ��
			gGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}
