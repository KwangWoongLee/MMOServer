#pragma once
#include "stdafx.h"
#include "IOCPSession.h"

class ZoneClientSessionManager;

class ZoneClientSession : public IOCPSession
{
public:
    explicit ZoneClientSession(ZoneClientSessionManager* manager);

protected:
    void OnConnected() override;
    void OnDisconnected() override;

private:
    ZoneClientSessionManager* _manager;
};