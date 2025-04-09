#pragma once
#include "stdafx.h"

#define THREAD_DIE 0


//using AcceptEx = SocketUtil::FnAcceptEx;
//extern LPFN_CONNECTEX FnConnectEx = nullptr;
//extern LPFN_DISCONNECTEX FnDisConnectEx = nullptr;

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
	uint16_t size;
	uint16_t id;
	uint64_t tickCount;
};