#include "stdafx.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "Memory.h"


ThreadManager* gThreadManager = nullptr;
GlobalQueue* gGlobalQueue = nullptr;
JobTimer* gJobTimer = nullptr;
Memory* gMemory = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		gThreadManager = new ThreadManager();
		gGlobalQueue = new GlobalQueue();
		gJobTimer = new JobTimer();
		gMemory = new Memory();
	}

	~CoreGlobal()
	{
		delete gThreadManager;
		delete gGlobalQueue;
		delete gJobTimer;
		delete gMemory;
	}
} gCoreGlobal;