#include "stdafx.h"
#include "ThreadLocalStorage.h"

thread_local uint32 TLS_ThreadId = 0;
thread_local uint64 TLS_LastTickCount = 0;
thread_local std::queue<SessionRef> TLS_SendSessionQueue;
thread_local CircularBuffer TLS_SendBuffer;
thread_local JobQueue* TLS_JobQueue = nullptr;