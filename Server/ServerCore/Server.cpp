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
		throw std::exception("서버가 완벽히 초기화 되지 않았습니다.");
	}

	mListener.Start();

	mState = eState::RUN;
}

void Server::Stop()
{
	if (mState != eState::RUN)
	{
		throw std::exception("서버가 작동중이지 않습니다.");
	}

	mListener.Stop();
	IOCP::GetInstance().Stop();

	mState = eState::STOP;
}
