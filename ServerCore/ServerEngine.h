#pragma once
#include "Listener.h"
#include "Engine.h"

class ServerEngine : public Engine
{
public:
	ServerEngine(std::string_view ip, uint16 port, std::shared_ptr<IOCP> iocp, uint16 maxSessionCount, SessionFactory sessionFactory);
	virtual ~ServerEngine() = default;

public:
	virtual bool Init() override;
	virtual void Run(uint32 timeout = INFINITE) override;
	virtual void Stop() override;

	SocketAddress& GetSockAddress() { return mSockAddr; };
	
private:
	std::string mIP;
	uint16 mPort;
	ListenerRef mListener = nullptr;
	SocketAddress mSockAddr = {};
};

