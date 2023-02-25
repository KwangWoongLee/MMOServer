#include "stdafx.h"
#include "Lock.h"
#include "ThreadLocalStorage.h"
#include "CoreMacro.h"

void Lock::WriteLock(const char* name)
{

	// 동일한 쓰레드가 소유하고 있다면 무조건 성공.
	const uint32 lockThreadId = (mLockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (TLS_ThreadId == lockThreadId)
	{
		mWriteCount++;
		return;
	}

	// 아무도 소유 및 공유하고 있지 않을 때, 경합해서 소유권을 얻는다.
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((TLS_ThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (mLockFlag.compare_exchange_strong(OUT expected, desired))
			{
				mWriteCount++;
				return;
			}
		}

		auto tick = GetTickCount64();
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
	// ReadLock 다 풀기 전에는 WriteUnlock 불가능.
	if ((mLockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --mWriteCount;
	if (lockCount == 0)
		mLockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공.
	const uint32 lockThreadId = (mLockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (TLS_ThreadId == lockThreadId)
	{
		mLockFlag.fetch_add(1);
		return;
	}

	// 아무도 소유하고 있지 않을 때 경합해서 공유 카운트를 올린다.
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (mLockFlag.load() & READ_COUNT_MASK);
			if (mLockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
	if ((mLockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
