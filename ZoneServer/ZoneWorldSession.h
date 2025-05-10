#pragma once
#include "stdafx.h"
#include "IOCPSession.h"

class ZoneWorldSessionManager;

class ZoneWorldSession : public IOCPSession
{
public:
    explicit ZoneWorldSession(ZoneWorldSessionManager* manager);

protected:
    void OnConnected() override;
    void OnDisconnected() override;

private:
    ZoneWorldSessionManager* _manager;
};