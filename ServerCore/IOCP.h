#pragma once

// Completion Port�� ���� ���� I/O �۾��� �ϴ� ��ü�� ��ӹ��� Ŭ����
class IOCPObject : public EnableShared<IOCPObject>
{
	// �߻�Ŭ����
public:
	virtual HANDLE GetHandle() = 0;
	virtual void Dispatch(class Overlapped* iocpEvent, uint32_t numOfBytes = 0) = 0;
};


class IOCP
{
public:
	IOCP();
	virtual ~IOCP();

	bool RegistForCompletionPort(IOCPObjectRef iocpObject);

	bool Init();
	void Run(uint32_t timeout = INFINITE);
	void Stop();

	// IOWorker ������ �Լ�
	void IOWorkerFunc(uint32_t timeout = INFINITE);
	
private:
	HANDLE mCompletionPort = INVALID_HANDLE_VALUE;

private:
	bool createCompletionPort();

};