#include "stdafx.h"
#include "Listener.h"
#include "IOCP.h"
#include "IOContext.h"
#include "ServerEngine.h"

#pragma region Listener Virtual

void Listener::Dispatch(Overlapped* iocpEvent, uint32 numOfBytes)
{
	// dynamic_cast�� �ٿ�ĳ���� �ؾ�������, Listener IOCP Object�� Accept �̺�Ʈ�� ó���ϱ⶧����
	// �׳� �̻��Ѱ� ������ ���α׷� ���������
	// ASSERT
	ASSERT_CRASH(iocpEvent->ioType == eIOType::ACCEPT)

	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	auto session = static_pointer_cast<Session>(acceptEvent->GetSession());

	SOCKET sessionSock = reinterpret_cast<SOCKET>(session->GetHandle());

	// Listen Socket�� ���� �ɼ� �״�� ��������
	if (SocketUtil::GetInstance().SetUpdateAcceptSocket(sessionSock, mListenSocket) == false)
	{
		auto error = WSAGetLastError();
		
		// �����ߴ��� Accept�� �ٽ� �ɾ��ֱ�
		asyncAccept(acceptEvent);
		return;
	}

	// Accept ������ ���� ���� ��������
	if (session->SetSockAddr() == false)
	{
		// �����ߴ��� Accept�� �ٽ� �ɾ��ֱ�
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

	// �ִ� ���� ������ŭ AcceptEx �س���
	prepareAccepts();

	return true;
}

bool Listener::bind()
{
	if ( ::bind(mListenSocket, (struct sockaddr*)&mServer->GetSockAddress().GetSockAddr(), sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		return false;

	return true;
}

bool Listener::listen(int32 backlog)
{
	if (::listen(mListenSocket, backlog) == SOCKET_ERROR)
		return false;

	return true;
}



void Listener::prepareAccepts()
{
	uint16 maxSessionCount = mServer->GetMaxSessionCount();
	mAcceptEvents.reserve(maxSessionCount);

	for (uint16 i{}; i < maxSessionCount; ++i)
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
