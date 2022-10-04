// 미리 컴파일된 헤더입니다.
#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dbghelp.lib")

#define NOMINMAX

// 윈도우 관련
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <mswsock.h>
#include <Mmsystem.h>
#include <Ws2spi.h>
#include <Mstcpip.h>
#include <windows.h>
#include <cassert>

// std
#include <iostream>
#include <stdexcept>
#include <format>
#include <memory>
#include <thread>
#include <mutex>

#include <functional>

//자료구조
#include <iostream>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <queue>


#include "Singleton.h"

#include "Types.h"
#include "Stream.h"
#include "PacketType.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "PacketAnalyzer.h"

#include "RecvBuffer.h"
#include "SendBuffer.h"

#include "IOContext.h"

#include "Listener.h"

#include "Session.h"
#include "SessionManager.h"

#include "IOCP.h"

//임시
//enum class eThreadType {
//	MAIN,
//	IO_WORKER,
//};
//
//thread_local eThreadType threadType;