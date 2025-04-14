#include "stdafx.h"
#include "Listener.h"
#include "IOCP.h"
#include "IOCPSessionManager.h"

void Listener::Dispatch(std::shared_ptr<Overlapped> const iocpEvent, uint32_t const numOfBytes)
{
	auto acceptEvent = std::dynamic_pointer_cast<AcceptEvent>(iocpEvent);
	if (!acceptEvent)
	{
		return;
	}

	auto const iocpSession = acceptEvent->GetIOCPSession();
	if (not SocketUtil::Singleton::Instance().SetUpdateAcceptSocket(reinterpret_cast<SOCKET>(iocpSession->GetHandle()), reinterpret_cast<SOCKET>(*GetHandle())))
	{
		asyncAccept(acceptEvent);
		return;
	}

	if (not iocpSession->SetSockAddr())
	{
		asyncAccept(acceptEvent);
		return;
	}

	iocpSession->OnAcceptCompleted();

	asyncAccept(acceptEvent); 
}

Listener::~Listener()
{
}

bool Listener::Init()
{
	auto const listenSocket = SocketUtil::Singleton::Instance().CreateSocket();
	if (not SocketUtil::Singleton::Instance().SetReuseAddress(listenSocket, true))
	{
		return false;
	}

	if (not SocketUtil::Singleton::Instance().SetLinger(listenSocket, 0, 0))
	{
		return false;
	}

	if (not SocketUtil::Singleton::Instance().Bind(listenSocket))
	{
		return false;
	}
	
	if (not SocketUtil::Singleton::Instance().Listen(listenSocket))
	{
		return false;
	}

	SetHandle(std::make_unique<HANDLE>(listenSocket));

	if (IOCPSessionManager::Singleton::Instance().RegistListener(shared_from_this()))
	{
		return false;
	}

	prepareAccepts();

	return true;
}


void Listener::prepareAccepts()
{
	auto const maxSessionCount = mServer->GetMaxSessionCount();
	for (uint16_t i{}; i < maxSessionCount; ++i)
	{
		auto const acceptEvent = ObjectPool<AcceptEvent>::Singleton::Instance().Acquire();
		asyncAccept(acceptEvent);
	}
}

void Listener::asyncAccept(std::shared_ptr<AcceptEvent> acceptEvent)
{
	acceptEvent->Init();

	auto const iocpSession = IOCPSessionManager::Singleton::Instance().CreateSession();
	acceptEvent->SetSession(iocpSession);

	DWORD bytesReceived = 0;
	if (not FnAcceptEx(reinterpret_cast<SOCKET>(*GetHandle()), reinterpret_cast<SOCKET>(*iocpSession->GetHandle()), iocpSession->mAcceptBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			ObjectPool<AcceptEvent>::Singleton::Instance().Release(acceptEvent);
		}
	}
}
