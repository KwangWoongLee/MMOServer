#pragma once


#include <windows.h>
#include <iostream>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


// std
#include <iostream>
#include <stdexcept>
#include <format>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>
#include <string>
#include <string_view>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <queue>
#include <condition_variable>

#include <shared_mutex>

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message_lite.h>


#include "RAII.h"
#include "Singleton.h"

#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "Container.h"
#include "ObjectPool.h"
#include "ThreadLocalStorage.h"
#include "LockQueue.h"
#include "Session.h"
