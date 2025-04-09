#include "stdafx.h"
#include "ThreadLocalStorage.h"

thread_local uint32_t TLS_ThreadId = 0;
thread_local uint64_t TLS_LastTickCount = 0;
thread_local std::set<SessionRef> TLS_SendSessions;
thread_local CircularBuffer TLS_SendBuffer(65535);
thread_local JobQueue* TLS_JobQueue = nullptr;