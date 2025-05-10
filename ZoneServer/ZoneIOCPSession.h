#pragma once
#include "stdafx.h"

#include "IOCPSession.h"

class ZoneClientSession final
	: public IOCPSession
{
protected:
    void OnConnected() override
    {
        // 클라이언트 초기화
    }

    void OnDisconnected() override
    {
        ZoneClientSessionManager::Instance().ReleaseSession(_sessionId);
    }
};