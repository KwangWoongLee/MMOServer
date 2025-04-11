#include "stdafx.h"
#include "IOCP.h"
#include "IOContext.h"

bool IOCP::RegistForCompletionPort(std::shared_ptr<IIOCPObject> const& iocpObject)
{
    if (auto const handle = ::CreateIoCompletionPort(iocpObject->GetHandle(), _completionPort, 0, 0); 
		handle != _completionPort)
	{
		return false;
	}

	return true;
}

void IOCP::Run(uint32_t const timeout)
{
	IOWorkerFunc(timeout);
}

void IOCP::Stop()
{	
	::PostQueuedCompletionStatus(_completionPort, 0, reinterpret_cast<ULONG_PTR>(nullptr), nullptr);
}

void IOCP::IOWorkerFunc(uint32_t const timeout)
{
	Overlapped* iocpEvent = nullptr;
	ULONG_PTR key = 0;
	DWORD dwTransferred = 0;

	bool ret = ::GetQueuedCompletionStatus(_completionPort, &dwTransferred, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeout);

	if (ret == true)
	{	
 		auto owner = iocpEvent->owner;
		owner->Dispatch(iocpEvent, dwTransferred);

	}
	else {
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

