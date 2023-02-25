#pragma once

// Completion Port를 통해 들어온 I/O 작업을 하는 주체가 상속받을 클래스
class IOCPObject : public EnableShared<IOCPObject>
{
	// 추상클래스
public:
	virtual HANDLE GetHandle() = 0;
	virtual void Dispatch(class Overlapped* iocpEvent, uint32 numOfBytes = 0) = 0;
};


class IOCP
{
public:
	IOCP();
	virtual ~IOCP();

	bool RegistForCompletionPort(IOCPObjectRef iocpObject);

	bool Init();
	void Run(uint32 timeout = INFINITE);
	void Stop();

	// IOWorker 쓰레드 함수
	void IOWorkerFunc(uint32 timeout = INFINITE);
	
private:
	HANDLE mCompletionPort = INVALID_HANDLE_VALUE;

private:
	bool createCompletionPort();

};