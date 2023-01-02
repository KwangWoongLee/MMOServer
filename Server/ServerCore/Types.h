#pragma once
#include "stdafx.h"

#define THREAD_DIE 0


static LPFN_ACCEPTEX FnAcceptEx = nullptr;
static LPFN_CONNECTEX FnConnectEx = nullptr;
static LPFN_DISCONNECTEX FnDisConnectEx = nullptr;

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
using IOCPRef = std::shared_ptr<class IOCP>;
using IOCPObjectRef = std::shared_ptr<class IOCPObject>;
using ListenerRef = std::shared_ptr<class Listener>;
using SessionRef = std::shared_ptr<class Session>;
using PacketSessionRef = std::shared_ptr<class PacketSession>;
using EngineRef = std::shared_ptr<class Engine>;
using ServerEngineRef = std::shared_ptr<class ServerEngine>;
using ClientEngineRef = std::shared_ptr<class ClientEngine>;
using SendBufferRef = std::shared_ptr<class SendBuffer>;
using SendBufferChunkRef = std::shared_ptr<class SendBufferChunk>;



template <typename T>
using EnableShared = std::enable_shared_from_this<T>;

//template<typename T, typename... Args>
//std::shared_ptr<T> MakeShared(Args&&... args)
//{
//	return std::shared_ptr<T>{ xnew<Type>(forward<Args>(args)...), xdelete<T> };
//}