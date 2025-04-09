#include "stdafx.h"
#include "Listener.h"
#include "IOCP.h"
#include "IOContext.h"
#include "ServerEngine.h"

#pragma region Listener Virtual

void Listener::Dispatch(Overlapped* iocpEvent, uint32_t numOfBytes)
{
	// dynamic_cast로 다운캐스팅 해야하지만, Listener IOCP Object는 Accept 이벤트만 처리하기때문에
	// 그냥 이상한게 들어오면 프로그램 뻗어버리기
	// ASSERT
	ASSERT_CRASH(iocpEvent->ioType == eIOType::ACCEPT)

	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	auto session = static_pointer_cast<Session>(acceptEvent->GetSession());

	SOCKET sessionSock = reinterpret_cast<SOCKET>(session->GetHandle());

	// Listen Socket이 가진 옵션 그대로 가져가기
	if (SocketUtil::GetInstance().SetUpdateAcceptSocket(sessionSock, mListenSocket) == false)
	{
		auto error = WSAGetLastError();
		
		// 실패했더라도 Accept는 다시 걸어주기
		asyncAccept(acceptEvent);
		return;
	}

	// Accept 소켓의 접속 정보 가져오기
	if (session->SetSockAddr() == false)
	{
		// 실패했더라도 Accept는 다시 걸어주기
		asyncAccept(acceptEvent);
		return;
	}

	session->OnAcceptCompleted();
	asyncAccept(acceptEvent);
}
#pragma endregion


Listener::Listener(ServerEngineRef server)
	: mServer(server)
{
}

Listener::~Listener()
{
	::closesocket(mListenSocket);
	mListenSocket = INVALID_SOCKET;
	mServer = nullptr;

	mAcceptEvents.clear();
}

bool Listener::Init()
{
	mListenSocket = SocketUtil::GetInstance().CreateSocket();

	if (mListenSocket == INVALID_SOCKET)
		return false;

	if (mServer->RegistForCompletionPort(shared_from_this()) == false)
		return false;

	if (SocketUtil::GetInstance().SetReuseAddress(mListenSocket, true) == false)
		return false;

	if (SocketUtil::GetInstance().SetLinger(mListenSocket, 0, 0) == false)
		return false;

	if (bind() == false)
		return false;
	

	if (listen() == false)
		return false;

	// 최대 세션 갯수만큼 AcceptEx 해놓기
	prepareAccepts();

	return true;
}

bool Listener::bind()
{
	auto serverSocketAddr = mServer->GetSockAddress().GetSockAddr();

	if ( ::bind(mListenSocket, (struct sockaddr*)&serverSocketAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		return false;

	return true;
}

bool Listener::listen(int32_t backlog)
{
	if (::listen(mListenSocket, backlog) == SOCKET_ERROR)
		return false;

	return true;
}



void Listener::prepareAccepts()
{
	uint16_t maxSessionCount = mServer->GetMaxSessionCount();
	mAcceptEvents.reserve(maxSessionCount);

	for (uint16_t i{}; i < maxSessionCount; ++i)
	{
		auto acceptEvent = new AcceptEvent();
		acceptEvent->owner = shared_from_this();

		mAcceptEvents.push_back(acceptEvent);
		asyncAccept(acceptEvent);
	}
	
}

void Listener::asyncAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = mServer->CreateSession(); 

	acceptEvent->Init();
	acceptEvent->SetSession(session);

	DWORD bytesReceived = 0;
	if (false == FnAcceptEx(mListenSocket, reinterpret_cast<SOCKET>(session->GetHandle()), session->mAcceptBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			asyncAccept(acceptEvent);
		}
	}
}
