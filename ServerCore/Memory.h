#pragma once
#include "Allocator.h"

class MemoryPool;


class Memory
{
	enum
	{
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256), // 총 풀 크기는  32바이트 32개 + 128바이트 16개 + 256바이트 8개 -> 56개
		MAX_ALLOC_SIZE = 4096 // 4096 이상은 Memory Pool 이용 X, 그냥 할당
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32_t size);
	void	Release(void* ptr);

private:
	vector<MemoryPool*> mPools;

	MemoryPool* mPoolTable[MAX_ALLOC_SIZE + 1];
};


template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // placement new
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type>{ xnew<Type>(forward<Args>(args)...), xdelete<Type> };
}