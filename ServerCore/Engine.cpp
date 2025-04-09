#include "stdafx.h"
#include "Engine.h"
#include "IOCP.h"

Engine::Engine(IOCPRef iocp, uint16_t maxSessionCount, SessionFactory sessionFactory)
	:mIOCP(iocp),
	mMaxSessionCount(maxSessionCount),
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

	if (mIOCP->Init() == false)
		return false;

	return true;
}

void Engine::Run(uint32_t timeout)
{
	mIOCP->Run(timeout);
}

void Engine::Stop()
{
}

SessionRef Engine::CreateSession()
{
	SessionRef session = mSessionFactory();
	session->SetEngine(shared_from_this());
	
	{
		WRITE_LOCK;

		mSessions.push_back(session);

		if (mIOCP->RegistForCompletionPort(session) == false)
			return nullptr;
	}


	return session;
}

void Engine::DisConnectSession(SessionRef session)
{
	{
		WRITE_LOCK;
		auto iter = std::find(mSessions.begin(), mSessions.end(), session);
		if (iter != mSessions.end())
		{
			std::iter_swap(iter, mSessions.end() - 1);

			mSessions.pop_back();
		}
		
	}
}

bool Engine::RegistForCompletionPort(IOCPObjectRef iocpObject)
{
	return mIOCP->RegistForCompletionPort(iocpObject);
}
