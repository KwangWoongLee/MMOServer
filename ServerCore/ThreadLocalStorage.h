#pragma once

extern thread_local uint32 TLS_ThreadId;
extern thread_local uint64 TLS_LastTickCount;
extern thread_local std::queue<SessionRef> TLS_SendSessionQueue;
extern thread_local JobQueue* TLS_JobQueue;