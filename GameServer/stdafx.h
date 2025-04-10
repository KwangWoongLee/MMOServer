#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#pragma comment(lib, "Redis\\Debug\\hiredis.lib")
#pragma comment(lib, "MySQL\\Debug\\libmysql.lib")
#else

#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#pragma comment(lib, "Redis\\Release\\hiredis.lib")
#pragma comment(lib, "MySQL\\Release\\libmysql.lib")
#endif // _DEBUG

#include <utility>
#include <algorithm>

#include "CorePch.h"

#include "Position.h"

#include "GameTypes.h"
#include "GameRandom.h"
