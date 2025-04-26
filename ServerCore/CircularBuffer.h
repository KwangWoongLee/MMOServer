#pragma once

class CircularBuffer final
{
public:
	CircularBuffer(uint32_t const capacity);
	virtual ~CircularBuffer();

	void BufferReset();
	uint32_t GetFreeSpaceSize();

	uint32_t GetStoredSize() const;
	uint32_t GetContiguiousBytes() const;
	char* GetBuffer();
	void Commit(uint32_t len);
	char* GetBufferStart();
	void Remove(uint32_t len);

private:
	void AllocateB();
	uint32_t GetAFreeSpace() const;
	uint32_t GetSpaceBeforeA() const;
	uint32_t GetBFreeSpace() const;

private:
	std::vector<char> mBuffer;
	uint32_t	mCapacity = 0x10000; // SEND : 65535

	uint32_t mARegionPos;
	uint32_t mARegionSize;

	uint32_t mBRegionPos;
	uint32_t mBRegionSize;
};