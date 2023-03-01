#pragma once
#include "stdafx.h"

#define THREAD_DIE 0


//using AcceptEx = SocketUtil::FnAcceptEx;
//extern LPFN_CONNECTEX FnConnectEx = nullptr;
//extern LPFN_DISCONNECTEX FnDisConnectEx = nullptr;

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

// shared_ptr
using JobRef = std::shared_ptr<class Job>;
using JobQueueRef = std::shared_ptr<class JobQueue>;
using IOCPRef = std::shared_ptr<class IOCP>;
using IOCPObjectRef = std::shared_ptr<class IOCPObject>;
using ListenerRef = std::shared_ptr<class Listener>;
using SessionRef = std::shared_ptr<class Session>;
using PacketSessionRef = std::shared_ptr<class PacketSession>;
using EngineRef = std::shared_ptr<class Engine>;
using ServerEngineRef = std::shared_ptr<class ServerEngine>;
using ClientEngineRef = std::shared_ptr<class ClientEngine>;
using CircularBufferRef = std::shared_ptr<class CircularBuffer>;



template <typename T>
using EnableShared = std::enable_shared_from_this<T>;


struct PacketHeader
{
	uint16 size;
	uint16 id;
	uint64 tickCount;
};

#define _STOMP