#pragma once

#define USE_LOCK				LOCK mLock;
#define READ_LOCK				ReadLockGuard readLockGuard(mLock, typeid(this).name());
#define WRITE_LOCK				WriteLockGuard writeLockGuard(mLock, typeid(this).name());

/*---------------
	  Crash
---------------*/

#define CRASH(cause)						\
{											\
	uint32_t* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}