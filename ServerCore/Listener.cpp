#include "stdafx.h"
#include "Listener.h"
#include "IOCP.h"
#include "IOCPSessionManager.h"

void Listener::Dispatch(std::shared_ptr<Overlapped> const iocpEvent, uint32_t const numOfBytes)
{
	if (EIOType::ACCEPT != iocpEvent->GetIOType())
		return;

	auto const iocpObject = iocpEvent->GetIOCPObject();
	auto const iocpSession = std::dynamic_pointer_cast<IOCPSession>(iocpObject);
	if (not iocpSession)
	{
		asyncAccept(); // 새 세션 + 새 Overlapped로
		return;
	}

	SOCKET acceptedSocket = reinterpret_cast<SOCKET>(iocpSession->GetHandle());
	SOCKET listenSocket = reinterpret_cast<SOCKET>(*GetHandle());

	if (not SocketUtil::Singleton::Instance().SetUpdateAcceptSocket(acceptedSocket, listenSocket))
	{
		asyncAccept(); // 실패 시에도 재시도
		return;
	}

	if (not iocpSession->SetSockAddr())
	{
		asyncAccept(); // 실패 시에도 재시도
		return;
	}

	iocpSession->OnAcceptCompleted(); // 연결 완료 처리

	asyncAccept(); // 다음 AcceptEx 등록 (새 Overlapped, 새 소켓)
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
		asyncAccept();
	}
}

void Listener::asyncAccept()
{
	auto const ioEvent = ObjectPool<Overlapped>::Singleton::Instance().Acquire();
	ioEvent->Init();
	ioEvent->SetIOType(EIOType::ACCEPT);

	auto const iocpSession = IOCPSessionManager::Singleton::Instance().CreateSession();
	ioEvent->SetIOCPObject(iocpSession);

	DWORD bytesReceived = 0;
	if (not FnAcceptEx(reinterpret_cast<SOCKET>(*GetHandle()), reinterpret_cast<SOCKET>(*iocpSession->GetHandle()), iocpSession->mAcceptBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, static_cast<LPOVERLAPPED>(&(*ioEvent))))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			
		}
	}
}
