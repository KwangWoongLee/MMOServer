#pragma once
#include "Listener.h"
#include "Engine.h"

class ClientEngine : public Engine
{
public:
	ClientEngine(std::string_view serverIP, uint16 serverPort, IOCPRef iocp, uint16 maxSessionCount, SessionFactory sessionFactory);
	virtual ~ClientEngine() = default;

public:
	virtual bool Init() override;
	virtual void Run(uint32 timeout = INFINITE) override;
	virtual void Stop() override;

	SocketAddress& GetServerSockAddr() { return mServerSockAddr; };

private:
	//std::string_view mServerIP;
	uint16 mServerPort;
	SocketAddress mServerSockAddr;
};

