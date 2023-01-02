#include "stdafx.h"
#include "Engine.h"
#include "IOCP.h"

Engine::Engine(std::string_view ip, uint16 port, std::unique_ptr<IOCP>&& iocp, uint16 maxSessionCount, uint8 threadCount, SessionFactory sessionFactory)
	:mIOCP(std::move(iocp)),
	mMaxSessionCount(maxSessionCount),
	mThreadCount(threadCount),
	mSessionFactory(sessionFactory)
{
}

Engine::~Engine()
{
	mIOCP = nullptr;
}

bool Engine::Init()
{
	// Init 작업이 실패하면, 프로그램을 종료할 것이므로 그냥 리턴하기로 하자.

	if (SocketUtil::GetInstance().Init() == false)
		return false;

	if (mIOCP->Init(mThreadCount) == false)
		return false;

	return true;
}

void Engine::Run()
{
	mIOCP->Run();
}

void Engine::Stop()
{
}

bool Engine::RegistForCompletionPort(IOCPObjectRef iocpObject)
{
	return mIOCP->RegistForCompletionPort(iocpObject);
}
