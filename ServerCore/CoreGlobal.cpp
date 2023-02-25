#include "stdafx.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "GlobalQueue.h"
#include "JobTimer.h"


ThreadManager* gThreadManager = nullptr;
GlobalQueue* gGlobalQueue = nullptr;
JobTimer* gJobTimer = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		gThreadManager = new ThreadManager();
		gGlobalQueue = new GlobalQueue();
		gJobTimer = new JobTimer();
	}

	~CoreGlobal()
	{
		delete gThreadManager;
		delete gGlobalQueue;
		delete gJobTimer;
	}
} gCoreGlobal;