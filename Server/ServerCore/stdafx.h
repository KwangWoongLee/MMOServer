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

//�ڷᱸ��
#include <iostream>
#include <vector>
#include <unordered_map>
#include <atomic>


#include "Singleton.h"

//�ӽ�
//enum class eThreadType {
//	MAIN,
//	IO_WORKER,
//};
//
//thread_local eThreadType threadType;