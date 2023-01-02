#include "stdafx.h"
#include "ThreadManager.h"


ThreadManager::ThreadManager()
{
	// 벡터의 메모리 이동을 줄이기 위해, 50개 공간을 만들어놓자
	mThreads.reserve(50);
}

ThreadManager::~ThreadManager()
{
	// 혹시 남았을지 몰라서 프로그램 종료 시 기다림
	this->Join();
}

void ThreadManager::Join()
{
	for (auto& thread : mThreads)
		if (thread.joinable()) thread.join();

	mThreads.clear();
}
