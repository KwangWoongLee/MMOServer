#include "stdafx.h"
#include "Allocator.h"
#include "Memory.h"


void* StompAllocator::Alloc(int32_t size)
{
	const int64_t pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64_t dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8_t*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const int64_t address = reinterpret_cast<int64_t>(ptr);
	const int64_t baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}

void* PoolAllocator::Alloc(int32_t size)
{
	return gMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	gMemory->Release(ptr);
}