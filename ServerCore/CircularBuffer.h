#pragma once

class CircularBuffer
{
public:
	CircularBuffer(uint32 capacity);
	virtual ~CircularBuffer();

	void BufferReset();
	uint32 GetFreeSpaceSize();

	uint32 GetStoredSize() const;
	uint32 GetContiguiousBytes() const;
	char* GetBuffer();
	void Commit(uint32 len);
	char* GetBufferStart();
	void Remove(uint32 len);

private:
	void AllocateB();
	uint32 GetAFreeSpace() const;
	uint32 GetSpaceBeforeA() const;
	uint32 GetBFreeSpace() const;

private:
	std::vector<char> mBuffer;
	uint32	mCapacity = 0x10000; // SEND : 65535

	uint32 mARegionPos;
	uint32	mARegionSize;

	uint32 mBRegionPos;
	uint32	mBRegionSize;
};