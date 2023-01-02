#include "stdafx.h"
#include "Listener.h"
#include "IOCP.h"
#include "IOContext.h"
#include "Server.h"

#pragma region Listener Virtual

void Listener::Dispatch(Overlapped* iocpEvent, uint32 numOfBytes)
{
	// dynamic_cast�� �ٿ�ĳ���� �ؾ�������, Listener IOCP Object�� Accept �̺�Ʈ�� ó���ϱ⶧����
	// �׳� �̻��Ѱ� ������ ���α׷� ���������
	// ASSERT

	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	auto session = acceptEvent->GetSession();

	SOCKET sessionSock = reinterpret_cast<SOCKET>(session->GetHandle());

	// Listen Socket�� ���� �ɼ� �״�� ��������
	if (SocketUtil::GetInstance().SetUpdateAcceptSocket(sessionSock, mListenSocket) == false)
	{
		// �����ߴ��� Accept�� �ٽ� �ɾ��ֱ�
		return;
	}

	// Accept ������ ���� ���� ��������
	if (session->SetSockAddr() == false)
	{
		// �����ߴ��� Accept�� �ٽ� �ɾ��ֱ�

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

	// �ִ� ���� ������ŭ AcceptEx �س���
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

