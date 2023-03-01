#pragma once
#include "Allocator.h"

class MemoryPool;


class Memory
{
	enum
	{
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256), // �� Ǯ ũ���  32����Ʈ 32�� + 128����Ʈ 16�� + 256����Ʈ 8�� -> 56��
		MAX_ALLOC_SIZE = 4096 // 4096 �̻��� Memory Pool �̿� X, �׳� �Ҵ�
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
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