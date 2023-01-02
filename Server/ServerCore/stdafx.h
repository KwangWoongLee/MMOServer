// 미리 컴파일된 헤더입니다.
#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dbghelp.lib")

#define NOMINMAX

// 윈도우 관련
#include <windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <Ws2tcpip.h>

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

#include "CorePch.h"

#include "SocketUtil.h"
#include "PacketType.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "PacketAnalyzer.h"


#include "CircularBuffer.h"
#include "SendBuffer.h"

#include "IOContext.h"

#include "Listener.h"

#include "IOCP.h"