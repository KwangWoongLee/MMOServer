#include "stdafx.h"


Stream::Stream()
	: mBuffer(nullptr)
	, mCapacity(0)
	, mHead(0)
{}


void Stream::ReallocBuffer(size_t size)
{
	mBuffer = static_cast<char*>(std::realloc(mBuffer, size));

	if (mBuffer == nullptr)
	{
		ASSERT(false);
	}

	mCapacity = size;
}

OutputStream::OutputStream()
	:Stream()
{
	ReallocBuffer(32);
}



void OutputStream::write(const void* data, size_t size)
{
	size_t resultHead = mHead + size;

	if (resultHead > mCapacity)
	{
		ReallocBuffer((std::max)(mCapacity * 2, resultHead));
	}

	memcpy_s((void*)(mBuffer + mHead), resultHead - mHead, data, size);

	mHead = resultHead;

}




InputStream::InputStream(char* buffer, size_t size)
	:Stream()
{
	mBuffer = buffer;
	mCapacity = size;
	mHead = 0;
}

void InputStream::read(void* data, size_t size)
{
	size_t resultHead = mHead + size;
	if (resultHead > mCapacity)
	{
		ASSERT(false);
	}

	std::memcpy(data, mBuffer + mHead, size);

	mHead = resultHead;
}
