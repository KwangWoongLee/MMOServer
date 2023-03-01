#include "stdafx.h"
#include "MemoryPool.h"


MemoryPool::MemoryPool(int32 allocSize) : mAllocSize(allocSize)
{
	::InitializeSListHead(&mHeader);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&mHeader)))
		::_aligned_free(memory);
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0;

	::InterlockedPushEntrySList(&mHeader, static_cast<PSLIST_ENTRY>(ptr));

	mUseCount.fetch_sub(1);
	mReserveCount.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&mHeader));

	// 없으면 새로 만들다
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(mAllocSize, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
		mReserveCount.fetch_sub(1);
	}

	mUseCount.fetch_add(1);

	return memory;
}
