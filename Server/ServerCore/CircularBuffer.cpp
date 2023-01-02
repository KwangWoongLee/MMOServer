#include "stdafx.h"
#include "CircularBuffer.h"

CircularBuffer::CircularBuffer() 
	: mARegionPos(0), mBRegionPos(UNKNOWN),mARegionSize(0), mBRegionSize(0)
{
	mBuffer.reserve(CAPACITY);
}

CircularBuffer::~CircularBuffer()
{
	mBuffer.clear();
}

void CircularBuffer::BufferReset()
{
	mARegionPos = 0;
	mBRegionPos = 0;
	mARegionSize = 0;
	mBRegionSize = 0;
}


void CircularBuffer::Remove(uint32 len)
{
	uint32 cnt = len;

	/// Read와 마찬가지로 A가 있다면 A영역에서 먼저 삭제

	if (mARegionSize > 0)
	{
		uint32 aRemove = (cnt > mARegionSize) ? mARegionSize : cnt;
		mARegionSize -= aRemove;
		mARegionPos += aRemove;
		cnt -= aRemove;
	}

	// 제거할 용량이 더 남은경우 B에서 제거 
	if (cnt > 0 && mBRegionSize > 0)
	{
		uint32 bRemove = (cnt > mBRegionSize) ? mBRegionSize : cnt;
		mBRegionSize -= bRemove;
		mBRegionPos += bRemove;
		cnt -= bRemove;
	}

	/// A영역이 비워지면 B를 A로 스위치 
	if (mARegionSize == 0)
	{
		if (mBRegionSize > 0)
		{
			/// 앞으로 당겨 붙이기
			if (mBRegionPos != UNKNOWN)
				std::move(mBuffer.begin() + mBRegionPos, mBuffer.begin() + mBRegionPos + mBRegionSize, mBuffer.begin());

			mARegionPos = mBRegionPos;
			mARegionSize = mBRegionSize;
			mBRegionPos = UNKNOWN;
			mBRegionSize = 0;
		}
		else
		{
			mBRegionPos = UNKNOWN;
			mBRegionSize = 0;
			mARegionPos = 0;
			mARegionSize = 0;
		}
	}
}


uint32 CircularBuffer::GetFreeSpaceSize()
{
	if (mBRegionPos != UNKNOWN)
		return GetBFreeSpace();
	else
	{
		/// A 버퍼보다 더 많이 존재하면, B 버퍼로 스위치
		if (GetAFreeSpace() < GetSpaceBeforeA())
		{
			AllocateB();
			return GetBFreeSpace();
		}
		else
			return GetAFreeSpace();
	}
}

uint32 CircularBuffer::GetStoredSize() const
{
	return mARegionSize + mBRegionSize;
}

uint32 CircularBuffer::GetContiguiousBytes() const
{
	if (mARegionSize > 0)
		return mARegionSize;
	else
		return mBRegionSize;
}

/// 쓰기가 가능한 위치 (버퍼의 끝부분) 반환
char* CircularBuffer::GetBuffer()
{
	if (mBRegionPos != UNKNOWN)
		return &mBuffer[static_cast<uint64>(mBRegionPos + mBRegionSize)];
	else
		return &mBuffer[static_cast<uint64>(mARegionPos + mARegionSize)];
}




/// 커밋(aka. IncrementWritten)
void CircularBuffer::Commit(uint32 len)
{
	if (mBRegionPos != UNKNOWN)
		mBRegionSize += len;
	else
		mARegionSize += len;
}

// 읽기 위치
char* CircularBuffer::GetBufferStart()
{
	if (mARegionSize > 0)
		return &mBuffer[mARegionPos];
	else
		return &mBuffer[mBRegionPos];
}

void CircularBuffer::AllocateB()
{
	mBRegionPos = 0;
}

uint32 CircularBuffer::GetAFreeSpace() const
{
	return (mBuffer.size() - mARegionPos - mARegionSize);
}

uint32 CircularBuffer::GetSpaceBeforeA() const
{
	return mARegionPos;
}


uint32 CircularBuffer::GetBFreeSpace() const
{
	if (mBRegionPos == UNKNOWN)
		return 0;

	return (mARegionPos - mBRegionPos - mBRegionSize);
}
