#include "stdafx.h"
#include "Memory.h"
#include "MemoryPool.h"


Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	// �ش� �������� MemoryPool�� �����Ͽ� MemoryPool Vector�� ����
	// mPoolTable�� ���� ��ȸ�� ���� ��, �� �ش� ������ �ܿ� �ǹ̾���
	// 
	// 32 64 96 ... 1024 ũ�� ������ 32 ������ ����
	// 32¥�� Ǯ, 64¥�� Ǯ...
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

	// 1024 1132 ... 2048 ũ�� ������ 128 ������ ����
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

	// 2048 2304 ... 4096 ũ�� ������ 256 ������ ����
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
		// �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� �Ҵ�
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// �޸� Ǯ���� �����´�
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
		// �޸� Ǯ�� �ִ� ũ�⸦ ����� �Ϲ� ����
		::_aligned_free(header);
	}
	else
	{
		// �޸� Ǯ�� �ݳ��Ѵ�
		mPoolTable[allocSize]->Push(header);
	}
#endif	
}
