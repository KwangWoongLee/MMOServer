#pragma once

class ThreadManager
{
public:
	ThreadManager();
	virtual ~ThreadManager();

	void AddThread(function<void(void)> callback);
	void Join();

	static void DoGlobalQueueWork();
	static void DistributeReservedJobs();

private:
	USE_LOCK; // ??

	std::vector<std::thread> mThreads;
	void initTLS();
	void stopTLS();
};

