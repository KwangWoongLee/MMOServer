#include "stdafx.h"
#include "ThreadManager.h"
#include "GlobalQueue.h"

ThreadManager::ThreadManager()
{
	mThreads.reserve(50);
}

ThreadManager::~ThreadManager()
{
	// Ȥ�� �������� ���� ���α׷� ���� �� ��ٸ�
	this->Join();
	mThreads.clear();
}

void ThreadManager::AddThread(function<void(void)> callback)
{
	{
		//LOCK 
		WRITE_LOCK;

		mThreads.push_back(thread([=]()
			{
				initTLS();
				callback();
				stopTLS();
			}));
	}
	
}


void ThreadManager::Join()
{
	for (auto& thread : mThreads)
		if (thread.joinable()) thread.join();

	mThreads.clear();
}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		uint64 now = ::GetTickCount64();
		if (now > TLS_LastTickCount)
			break;

		JobQueueRef jobQueue = gGlobalQueue->Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();
	}
}

void ThreadManager::DistributeReservedJobs()
{
	const uint64 now = ::GetTickCount64();

	gJobTimer->Distribute(now);
}

void ThreadManager::initTLS()
{
	static std::atomic<uint32> SThreadId = 1;
	TLS_ThreadId = SThreadId++;
}

void ThreadManager::stopTLS()
{
	if (TLS_JobQueue != nullptr)
	{
		TLS_JobQueue->Clear();
		delete TLS_JobQueue;
	}

	TLS_JobQueue = nullptr;
}
