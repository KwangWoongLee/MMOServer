#include "stdafx.h"
#include "IOCP.h"
#include "IOEvent.h"

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

	auto const result = ::GetQueuedCompletionStatus(_completionPort, &dwTransferred, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeout);
	if (result)
	{// 정상적인 io event 발생
		auto const iocpEventSptr = std::make_shared<Overlapped>(iocpEvent);

		auto const iocpObject = iocpEventSptr->GetIOCPObject();
		if (not iocpObject)
		{
			return;
		}
		iocpObject->Dispatch(iocpEventSptr, dwTransferred);
	}
	else 
	{
		// case 1. pending
		// case 2. timeout
		// case 3. error

		auto const err = WSAGetLastError();
		if (WSA_IO_PENDING == err)
		{
			return;
		}
		else if (not iocpEvent)
		{ // timeout
			return;
		}
		else
		{
			if (ERROR_NETNAME_DELETED == err || ERROR_CONNECTION_ABORTED == err)
			{ // 
				
			}

			auto const iocpEventSptr = std::make_shared<Overlapped>(iocpEvent);

			auto const iocpObject = iocpEventSptr->GetIOCPObject();
			if (not iocpObject)
			{
				return;
			}
			iocpObject->Dispatch(iocpEventSptr, dwTransferred);
		}
	}
	
}

