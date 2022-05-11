#pragma once
class IOCP : public Singleton< IOCP >
{
public:
	IOCP() {};
	~IOCP() {};


	void createCompletionPort();
	void RegistCompletionPort(SOCKET socket, ULONG_PTR key);
	
	void Start();
	void Stop();
	
	
private:
	HANDLE mCompletionPort;
	std::vector<std::thread> mIOWorkerThreadPool;

	void createThreadPool(short threadCount);
	void stopThreads();

	// IOWorker ������ �Լ�
	void IOWorkerFunc();
};