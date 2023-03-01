#include "stdafx.h"
#include "ServerEngine.h"
#include "IOCP.h"

ServerEngine::ServerEngine(std::string_view ip, uint16 port, std::shared_ptr<IOCP> iocp, uint16 maxSessionCount, SessionFactory sessionFactory)
	: Engine(iocp, maxSessionCount, sessionFactory), 
	mSockAddr(SocketAddress(port)),
	mIP(ip),
	mPort(port)
{
}


bool ServerEngine::Init()
{
	if (Engine::Init() == false)
		return false;

	//다운 캐스팅이지만 shared_ptr만 얻는 것이므로 static_pointer_cast로 사용
	auto serverEngineRef = static_pointer_cast<ServerEngine>(shared_from_this());
	mListener = MakeShared<Listener>(serverEngineRef);

	if (mListener->Init() == false)
		return false;

	return true;
}

void ServerEngine::Run(uint32 timeout)
{

	Engine::Run(timeout);
}

void ServerEngine::Stop()
{
	mListener = nullptr;

	Engine::Stop();
}
