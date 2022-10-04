#include "stdafx.h"

RecvBuffer::RecvBuffer(size_t bufferSize)
{
	mBufferSize = bufferSize;
	mBuffer = new char[bufferSize];
}

void RecvBuffer::Clean()
{
    size_t dataSize = GetDataSize();
    if (dataSize == 0)
    {
        //남은 데이터가 없으면 위치 리셋
        mReadPos = mWritePos = 0;
    }
    else
    {
        std::copy(mBuffer + mReadPos, mBuffer + mReadPos + dataSize + 1, mBuffer);
        
        mReadPos = 0;
        mWritePos = dataSize;
    }
}

bool RecvBuffer::OnRead(size_t numOfBytes)
{
    if (numOfBytes > GetDataSize())
        return false;

    mReadPos += numOfBytes;
    return true;
}

bool RecvBuffer::OnWrite(size_t numOfBytes)
{
    if (numOfBytes > GetFreeSize())
        return false;

    mWritePos += numOfBytes;
    return true;
}
