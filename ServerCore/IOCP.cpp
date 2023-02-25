#include "stdafx.h"
#include "IOCP.h"
#include "IOContext.h"

IOCP::IOCP()
{
}

IOCP::~IOCP()
{
}

bool IOCP::createCompletionPort()
{
	// ���ʿ� �ѹ��� CompletionPort �ʱ�ȭ
	mCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (mCompletionPort == nullptr)
		return false;

	return true;
}


bool IOCP::RegistForCompletionPort(IOCPObjectRef iocpObject)
{
	// Accept�� ���� ��, CompletionPort�� ���
	HANDLE handle = ::CreateIoCompletionPort(iocpObject->GetHandle(), mCompletionPort, 0, 0);

	// �����ϸ� CompletionPort�� ����
	if (handle != mCompletionPort)
		return false;

	return true;
}


bool IOCP::Init()
{
	return createCompletionPort();
}

void IOCP::Run(uint32 timeout)
{
	IOWorkerFunc(timeout);
}

void IOCP::Stop()
{	
	// PostQueuedCompletionStatus �� IOCP�� ���� ��ȣ�� ��
	BOOL ret = ::PostQueuedCompletionStatus(mCompletionPort, 0, THREAD_DIE, NULL);
}

void IOCP::IOWorkerFunc(uint32 timeout)
{
	Overlapped* iocpEvent = nullptr;
	ULONG_PTR key = 0;
	DWORD dwTransferred = 0;

	bool ret = ::GetQueuedCompletionStatus(mCompletionPort, &dwTransferred, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeout);
	// �����ϸ�  TRUE�� ��ȯ�ϰ� �׷��� ������ FALSE�� ��ȯ�մϴ�.

	if (ret == true) // ����
	{	
 		auto owner = iocpEvent->owner;
		owner->Dispatch(iocpEvent, dwTransferred);

	}
	else { // ����
		auto err = WSAGetLastError();
		if (err == WAIT_TIMEOUT)
			return;

		else
		{
			auto owner = iocpEvent->owner;
			owner->Dispatch(iocpEvent, dwTransferred);
		}
	}
	
}

