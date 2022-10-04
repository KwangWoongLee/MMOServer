#pragma once
#include "stdafx.h"

constexpr size_t CHUNK_SIZE = 4096;

class SendBuffer
{
public:
    SendBuffer(int chunkSize)
    {
        mBuffer = new char[chunkSize];
        mBufferSize = chunkSize;
    }
    virtual ~SendBuffer() { delete[] mBuffer; };

    size_t GetFreeSize() const { return mBufferSize - mUsedSize; };

    char* Open(size_t reserveSize)
    {
        if (reserveSize > GetFreeSize())
            return nullptr;

        return mBuffer + mUsedSize;
    }

    char* Close(size_t usedSize)
    {
        char* ret = mBuffer - mUsedSize;
        mUsedSize += usedSize;
        return ret;
    }

private:
    char* mBuffer = nullptr;
    size_t mBufferSize;
    size_t mUsedSize = 0;

};

static class SendBufferHelper
{
public:
    static thread_local SendBuffer* currentBuffer;

    static char* Open(int reserveSize)
    {
        if (currentBuffer == nullptr)
            currentBuffer = new SendBuffer(CHUNK_SIZE);
        else {
            if (currentBuffer->GetFreeSize() < reserveSize)
            {
                delete[] currentBuffer;
                currentBuffer = new SendBuffer(CHUNK_SIZE);
            }
            
        }
        
        return currentBuffer->Open(reserveSize);
    }

    static char* Close(int usedSize)
    {
        return currentBuffer->Close(usedSize);
    }
};
