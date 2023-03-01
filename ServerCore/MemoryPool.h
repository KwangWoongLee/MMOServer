#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};


DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(int32 size) : allocSize(size) { }

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		// (여기가 header 위치)header data 
		// ++포인터는 해당 포인터 크기만큼 메모리 위치 증가한 실제 data 시작 위치 
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		// header (여기가 ptr 위치)data 
		// 포인터 -1 은 해당 포인터 크기만큼 메모리 위치 감소한 MemoryHeader 시작 위치 
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};

/*-----------------
	MemoryPool
------------------*/

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	SLIST_HEADER	mHeader;
	int32			mAllocSize = 0;
	atomic<int32>	mUseCount = 0;
	atomic<int32>	mReserveCount = 0;
};

