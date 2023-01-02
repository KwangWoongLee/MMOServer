// �̸� �����ϵ� ����Դϴ�.
#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dbghelp.lib")

#define NOMINMAX

// ������ ����
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

//�ڷᱸ��
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