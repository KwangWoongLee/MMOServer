#include "stdafx.h"
#include "Memory.h"
#include "MemoryPool.h"


Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	// 해당 사이즈의 MemoryPool을 생성하여 MemoryPool Vector를 생성
	// mPoolTable은 빠른 조회를 위할 뿐, 그 해당 사이즈 외엔 의미없음
	// 
	// 32 64 96 ... 1024 크기 전까지 32 단위로 생성
	// 32짜리 풀, 64짜리 풀...
	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		mPools.push_back(pool);

		while (tableIndex <= size)
		{
			mPoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 1024 1132 ... 2048 크기 전까지 128 단위로 생성
	for (; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		mPools.push_back(pool);

		while (tableIndex <= size)
		{
			mPoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// 2048 2304 ... 4096 크기 전까지 256 단위로 생성
	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		mPools.push_back(pool);

		while (tableIndex <= size)
		{
			mPoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

Memory::~Memory()
{
	for (MemoryPool* pool : mPools)
		delete pool;

	mPools.clear();
}

void* Memory::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);

#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// 메모리 풀에서 꺼내온다
		header = mPoolTable[allocSize]->Pop();
	}
#endif	

	return MemoryHeader::AttachHeader(header, allocSize);
}

void Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 해제
		::_aligned_free(header);
	}
	else
	{
		// 메모리 풀에 반납한다
		mPoolTable[allocSize]->Push(header);
	}
#endif	
}
