#pragma once
class IOCP : public Singleton< IOCP >
{
public:
	IOCP() : mCompletionPort(nullptr) {};
	~IOCP() {};


	void createCompletionPort();
	void RegistCompletionPort(SOCKET socket, ULONG_PTR key);
	
	void Start(short threadCount);
	void Stop();
	
	
private:
	HANDLE mCompletionPort = nullptr;
	std::vector<std::thread> mIOWorkerThreadPool;

	void createThreadPool(short threadCount);
	void stopThreads();

	// IOWorker 쓰레드 함수
	void IOWorkerFunc();
};