#include "stdafx.h"
#include "Server.h"
#include "IOCP.h"

ServerEngine::ServerEngine(std::string_view ip, uint16 port, std::unique_ptr<IOCP>&& iocp, uint16 maxSessionCount, uint8 threadCount, SessionFactory sessionFactory)
	: Engine(ip, port, std::move(iocp), maxSessionCount, threadCount, sessionFactory)
{
}


bool ServerEngine::Init()
{
	if (Engine::Init() == false)
		return false;

	//다운 캐스팅이지만 shared_ptr만 얻는 것이므로 static_pointer_cast로 사용
	auto serverEngineRef = static_pointer_cast<ServerEngine>(shared_from_this());
	mListener = std::make_shared<Listener>(serverEngineRef);

	if (mListener->Init() == false)
		return false;

	return true;
}

void ServerEngine::Run()
{
	while (true)
	{
		// IOCP 루프
		Engine::Run(); 
	}
}

void ServerEngine::Stop()
{
	mListener = nullptr;

	Engine::Stop();
}
