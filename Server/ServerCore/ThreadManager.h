#pragma once

class ThreadManager : public Singleton <ThreadManager>
{
public:
	ThreadManager();
	virtual ~ThreadManager();

	void Join();

private:
	std::vector<std::thread> mThreads;


};

