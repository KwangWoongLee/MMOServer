#include "stdafx.h"
#include "IOCP.h"
#include "IOContext.h"

IOCP::IOCP()
{
}

IOCP::~IOCP()
{
	//// stop �Ȱ�ġ�� �������� �־ �ѹ� �� ���ֱ�
	//for (auto& thread : mIOWorkerThreadPool)
	//{
	//	if (thread.joinable())
	//		thread.join();
	//}

	//mCompletionPort = INVALID_HANDLE_VALUE;
	//mIOWorkerThreadPool.clear();
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


bool IOCP::Init(uint8 threadCount)
{
	return createCompletionPort();
}

void IOCP::Run()
{

}

void IOCP::Stop()
{	
	// PostQueuedCompletionStatus �� IOCP�� ���� ��ȣ�� ��
	BOOL ret = ::PostQueuedCompletionStatus(mCompletionPort, 0, THREAD_DIE, NULL);

	//if (!ret)
	//	throw std::format("Stop IOCP Failed {}", GetLastError());

	//mIOWorkerThreadPool.reserve(threadCount);


	//for (size_t i{}; i < mIOWorkerThreadPool.capacity(); ++i)
	//	mIOWorkerThreadPool.push_back(std::thread(&IOCP::IOWorkerFunc, 10));


	//for (auto& thread : mIOWorkerThreadPool)
	//{
	//	if (thread.joinable())
	//		thread.join();
	//}
}

void IOCP::IOWorkerFunc(uint32 timeout)
{
	while (true)
	{
		Overlapped* iocpEvent = nullptr;
		ULONG_PTR key = 0;
		DWORD dwTransferred = 0;

		bool ret = ::GetQueuedCompletionStatus(mCompletionPort, &dwTransferred, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), static_cast<DWORD>(timeout));
		// �����ϸ�  TRUE�� ��ȯ�ϰ� �׷��� ������ FALSE�� ��ȯ�մϴ�.

		if (ret == true) // ����
		{
			iocpEvent->GetOwner()->Dispatch(iocpEvent, dwTransferred);

		}
		else { // ����
			auto err = WSAGetLastError();
			if (err == WAIT_TIMEOUT)
				return;

			else
			{
				eIOType type = iocpEvent->GetType();
				iocpEvent->GetOwner()->Dispatch(iocpEvent, dwTransferred);

				break;
			}
		}
	}
}
