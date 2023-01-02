#include "stdafx.h"
#include "ThreadManager.h"


ThreadManager::ThreadManager()
{
	// ������ �޸� �̵��� ���̱� ����, 50�� ������ ��������
	mThreads.reserve(50);
}

ThreadManager::~ThreadManager()
{
	// Ȥ�� �������� ���� ���α׷� ���� �� ��ٸ�
	this->Join();
}

void ThreadManager::Join()
{
	for (auto& thread : mThreads)
		if (thread.joinable()) thread.join();

	mThreads.clear();
}
