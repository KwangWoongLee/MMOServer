#include "stdafx.h"
#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(uint32_t capacity) 
	: mCapacity(capacity), mARegionPos(0), mBRegionPos(mCapacity + 1), mARegionSize(0), mBRegionSize(0)
{
	mBuffer = Vector<char>(mCapacity);
}

CircularBuffer::~CircularBuffer()
{
}

void CircularBuffer::BufferReset()
{
	mARegionPos = 0;
	mBRegionPos = 0;
	mARegionSize = 0;
	mBRegionSize = 0;
}


void CircularBuffer::Remove(uint32_t len)
{
	uint32_t cnt = len;

	/// Read�� ���������� A�� �ִٸ� A�������� ���� ����

	if (mARegionSize > 0)
	{
		uint32_t aRemove = (cnt > mARegionSize) ? mARegionSize : cnt;
		mARegionSize -= aRemove;
		mARegionPos += aRemove;
		cnt -= aRemove;
	}

	// ������ �뷮�� �� ������� B���� ���� 
	if (cnt > 0 && mBRegionSize > 0)
	{
		uint32_t bRemove = (cnt > mBRegionSize) ? mBRegionSize : cnt;
		mBRegionSize -= bRemove;
		mBRegionPos += bRemove;
		cnt -= bRemove;
	}

	/// A������ ������� B�� A�� ����ġ 
	if (mARegionSize == 0)
	{
		if (mBRegionSize > 0)
		{
			/// ������ ��� ���̱�
			if (mBRegionPos != mCapacity + 1)
				std::move(mBuffer.begin() + mBRegionPos, mBuffer.begin() + mBRegionPos + mBRegionSize, mBuffer.begin());

			mARegionPos = mBRegionPos;
			mARegionSize = mBRegionSize;
			mBRegionPos = mCapacity + 1;
			mBRegionSize = 0;
		}
		else
		{
			mBRegionPos = mCapacity + 1;
			mBRegionSize = 0;
			mARegionPos = 0;
			mARegionSize = 0;
		}
	}
}


uint32_t CircularBuffer::GetFreeSpaceSize()
{
	if (mBRegionPos != mCapacity + 1)
		return GetBFreeSpace();
	else
	{
		/// A ���ۺ��� �� ���� �����ϸ�, B ���۷� ����ġ
		if (GetAFreeSpace() < GetSpaceBeforeA())
		{
			AllocateB();
			return GetBFreeSpace();
		}
		else
			return GetAFreeSpace();
	}
}

uint32_t CircularBuffer::GetStoredSize() const
{
	return mARegionSize + mBRegionSize;
}

uint32_t CircularBuffer::GetContiguiousBytes() const
{
	if (mARegionSize > 0)
		return mARegionSize;
	else
		return mBRegionSize;
}

/// ���Ⱑ ������ ��ġ (������ ���κ�) ��ȯ
char* CircularBuffer::GetBuffer()
{
	if (mBRegionPos != mCapacity + 1)
		return &mBuffer[static_cast<uint64_t>(mBRegionPos + mBRegionSize)];
	else
		return &mBuffer[static_cast<uint64_t>(mARegionPos + mARegionSize)];
}




/// Ŀ��(aka. IncrementWritten)
void CircularBuffer::Commit(uint32_t len)
{
	if (mBRegionPos != mCapacity + 1)
		mBRegionSize += len;
	else
		mARegionSize += len;
}

// �б� ��ġ
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

uint32_t CircularBuffer::GetAFreeSpace() const
{
	return (mBuffer.size() - mARegionPos - mARegionSize);
}

uint32_t CircularBuffer::GetSpaceBeforeA() const
{
	return mARegionPos;
}


uint32_t CircularBuffer::GetBFreeSpace() const
{
	if (mBRegionPos == mCapacity + 1)
		return 0;

	return (mARegionPos - mBRegionPos - mBRegionSize);
}
