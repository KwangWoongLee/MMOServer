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

	//�ٿ� ĳ���������� shared_ptr�� ��� ���̹Ƿ� static_pointer_cast�� ���
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
		// IOCP ����
		Engine::Run(); 
	}
}

void ServerEngine::Stop()
{
	mListener = nullptr;

	Engine::Stop();
}
