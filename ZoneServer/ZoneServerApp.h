#pragma once
#include "stdafx.h"
#include "App.h"
#include "Connector.h"
#include "Listener.h"

#include "ZoneClientSessionManager.h"
#include "ZoneWorldSessionManager.h"

class ZoneServerApp final
	: public IApp
{
public:
    bool Init() override;
    void Run() override;
    void Stop() override;

    ZoneClientSessionManager* GetClientSessionManager() const
    {
        return _clientSessionManager.get();
    }

    ZoneWorldSessionManager* GetWorldSessionManager() const
    {
        return _worldSessionManager.get();
    }

private:
    std::shared_ptr<IOCP> _iocp;
    std::shared_ptr<Listener> _listener;
    std::shared_ptr<Connector> _connector;

    std::unique_ptr<ZoneClientSessionManager> _clientSessionManager;
    std::unique_ptr<ZoneWorldSessionManager> _worldSessionManager;

    std::vector<std::thread> _workerThreads;
};