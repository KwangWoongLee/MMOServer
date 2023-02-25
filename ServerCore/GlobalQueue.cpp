#include "stdafx.h"
#include "GlobalQueue.h"

GlobalQueue::GlobalQueue()
{

}

GlobalQueue::~GlobalQueue()
{

}

void GlobalQueue::Push(JobQueueRef jobQueue)
{
	mJobQueues.Push(jobQueue);
}

JobQueueRef GlobalQueue::Pop()
{
	return mJobQueues.Pop();
}
