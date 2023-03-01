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
		// (���Ⱑ header ��ġ)header data 
		// ++�����ʹ� �ش� ������ ũ�⸸ŭ �޸� ��ġ ������ ���� data ���� ��ġ 
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		// header (���Ⱑ ptr ��ġ)data 
		// ������ -1 �� �ش� ������ ũ�⸸ŭ �޸� ��ġ ������ MemoryHeader ���� ��ġ 
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

