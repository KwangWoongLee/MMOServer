#pragma once

extern thread_local uint32_t TLS_ThreadId;
extern thread_local uint64_t TLS_LastTickCount;
extern thread_local std::set<SessionRef> TLS_SendSessions;
extern thread_local JobQueue* TLS_JobQueue;