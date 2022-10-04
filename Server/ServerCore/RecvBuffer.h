#pragma once
#include "stdafx.h"

class RecvBuffer
{
public:
    RecvBuffer(size_t bufferSize);
    virtual ~RecvBuffer() { delete[] mBuffer; };

    size_t GetDataSize() const { return mWritePos - mReadPos; };
    size_t GetFreeSize() const { return mBufferSize - mWritePos; };

    char* GetRecvLocation() const { return mBuffer + mReadPos; };
    char* GetWriteLocation() const { return mBuffer + mWritePos; };

    void Clean();

    bool OnRead(size_t numOfBytes);
    bool OnWrite(size_t numOfBytes);

private:
    char* mBuffer;
    size_t mBufferSize;
    size_t mReadPos;
    size_t mWritePos;

};