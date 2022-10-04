#include "stdafx.h"
#include "Server.h"
#include "IOCP.h"

void Server::Init(short port, short threadCount)
{
	IOCP::GetInstance().Start(threadCount);
	mListener.Init(port);

	mState = eState::INIT;
}

void Server::Run()
{
	if (mState != eState::INIT)
	{
		throw std::exception("������ �Ϻ��� �ʱ�ȭ ���� �ʾҽ��ϴ�.");
	}

	mListener.Start();

	mState = eState::RUN;
}

void Server::Stop()
{
	if (mState != eState::RUN)
	{
		throw std::exception("������ �۵������� �ʽ��ϴ�.");
	}

	mListener.Stop();
	IOCP::GetInstance().Stop();

	mState = eState::STOP;
}
