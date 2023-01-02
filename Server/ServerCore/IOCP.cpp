#include "stdafx.h"
#include "IOCP.h"
#include "IOContext.h"

IOCP::IOCP()
{
}

IOCP::~IOCP()
{
	//// stop 안거치고 왔을수도 있어서 한번 더 해주기
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
	// 최초에 한번만 CompletionPort 초기화
	mCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (mCompletionPort == nullptr)
		return false;

	return true;
}


bool IOCP::RegistForCompletionPort(IOCPObjectRef iocpObject)
{
	// Accept가 들어올 때, CompletionPort에 등록
	HANDLE handle = ::CreateIoCompletionPort(iocpObject->GetHandle(), mCompletionPort, 0, 0);

	// 성공하면 CompletionPort를 리턴
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
	// PostQueuedCompletionStatus 로 IOCP에 종료 신호를 줌
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
		// 성공하면  TRUE를 반환하고 그렇지 않으면 FALSE를 반환합니다.

		if (ret == true) // 성공
		{
			iocpEvent->GetOwner()->Dispatch(iocpEvent, dwTransferred);

		}
		else { // 실패
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
