#include "stdafx.h"
#include "Server.h"
#include "IOCP.h"

void Server::Init()
{
	IOCP::GetInstance().Start();
	mListener.Init(7777);

	mState = eState::INIT;
}

void Server::Run()
{
	mListener.Start();

	mState = eState::RUN;
}

void Server::Stop()
{
	mListener.Stop();
	IOCP::GetInstance().Stop();

	mState = eState::STOP;
}
