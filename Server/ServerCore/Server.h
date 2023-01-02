#pragma once
#include "Listener.h"
#include "Engine.h"

class ServerEngine : public Engine
{
public:
	ServerEngine(std::string_view ip, uint16 port, std::unique_ptr<IOCP>&& iocp, uint16 maxSessionCount, uint8 threadCount, SessionFactory sessionFactory);
	virtual ~ServerEngine() = default;

public:
	virtual bool Init() override;
	virtual void Run() override;
	virtual void Stop() override;
	
private:
	ListenerRef mListener = nullptr;
};

