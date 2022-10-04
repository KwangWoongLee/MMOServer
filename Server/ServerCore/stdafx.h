// �̸� �����ϵ� ����Դϴ�.
#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dbghelp.lib")

#define NOMINMAX

// ������ ����
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

//�ڷᱸ��
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

//�ӽ�
//enum class eThreadType {
//	MAIN,
//	IO_WORKER,
//};
//
//thread_local eThreadType threadType;