#pragma once
#include "Types.h"


using LOCK = class Lock; // 나중에 여러 Lock 테스트
/*--------------------------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W : WriteFlag (Exclusive Lock Owner ThreadId)
R : ReadFlag (Shared Lock Count)
---------------------------------------------*/

class Lock
{
    enum : uint32_t
    {
        ACQUIRE_TIMEOUT_TICK = 10000,
        MAX_SPIN_COUNT = 5000,
        WRITE_THREAD_MASK = 0xFFFF'0000,
        READ_COUNT_MASK = 0x0000'FFFF,
        EMPTY_FLAG = 0x0000'0000
    };

public:
    void WriteLock(const char* name);
    void WriteUnlock(const char* name);
    void ReadLock(const char* name);
    void ReadUnlock(const char* name);

private:
    std::atomic<uint32_t> mLockFlag = EMPTY_FLAG;
    uint16_t mWriteCount = 0;
};



/*----------------
    LockGuards
-----------------*/

class ReadLockGuard
{
public:
    ReadLockGuard(LOCK& lock, const char* name) : mLock(lock), mName(name) { mLock.ReadLock(name); }
    ~ReadLockGuard() { mLock.ReadUnlock(mName); }

private:
    LOCK& mLock;
    const char* mName;
};

class WriteLockGuard
{
public:
    WriteLockGuard(LOCK& lock, const char* name) : mLock(lock), mName(name) { mLock.WriteLock(name); }
    ~WriteLockGuard() { mLock.WriteUnlock(mName); }

private:
    LOCK& mLock;
    const char* mName;
};