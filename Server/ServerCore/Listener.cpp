#include "stdafx.h"
#include "Listener.h"
#include "IOCP.h"
#include "IOContext.h"
#include "Server.h"

#pragma region Listener Virtual

void Listener::Dispatch(Overlapped* iocpEvent, uint32 numOfBytes)
{
	// dynamic_cast로 다운캐스팅 해야하지만, Listener IOCP Object는 Accept 이벤트만 처리하기때문에
	// 그냥 이상한게 들어오면 프로그램 뻗어버리기
	// ASSERT

	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	auto session = acceptEvent->GetSession();

	SOCKET sessionSock = reinterpret_cast<SOCKET>(session->GetHandle());

	// Listen Socket이 가진 옵션 그대로 가져가기
	if (SocketUtil::GetInstance().SetUpdateAcceptSocket(sessionSock, mListenSocket) == false)
	{
		// 실패했더라도 Accept는 다시 걸어주기
		return;
	}

	// Accept 소켓의 접속 정보 가져오기
	if (session->SetSockAddr() == false)
	{
		// 실패했더라도 Accept는 다시 걸어주기

		return;
	}

	session->OnAcceptCompleted();
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
	return false;
}

void Listener::prepareAccepts()
{
	uint16 maxSessionCount = mServer->GetMaxSessionCount();
	mAcceptEvents.reserve(maxSessionCount);

	for (uint16 i{}; i < maxSessionCount; ++i)
	{
		auto acceptEvent = new AcceptEvent();
		acceptEvent->SetOwner(shared_from_this());

		mAcceptEvents.push_back(acceptEvent);
	}
	
}

bool Listener::listen(int32 backlog)
{
	return false;
}

