#pragma once

// Completion Port를 통해 들어온 I/O 작업을 하는 주체가 상속받을 클래스
class IOCPObject : public EnableShared<IOCPObject>
{
	// 추상클래스
public:
	virtual HANDLE GetHandle() const = 0;
	virtual void Dispatch(class Overlapped* iocpEvent, uint32 numOfBytes = 0) = 0;
};


class IOCP
{
public:
	IOCP();
	virtual ~IOCP();

	bool RegistForCompletionPort(IOCPObjectRef iocpObject);

	bool Init(uint8 threadCount);
	void Run();
	void Stop();
	
	
private:
	HANDLE mCompletionPort = INVALID_HANDLE_VALUE;

private:
	bool createCompletionPort();

	// IOWorker 쓰레드 함수
	void IOWorkerFunc(uint32 timeout = INFINITE);
};