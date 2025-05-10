#include "stdafx.h"
#include "ZoneClientSession.h"
#include "ZoneClientSessionManager.h"

ZoneClientSession::ZoneClientSession(ZoneClientSessionManager* manager)
    : _manager(manager)
{
}

void ZoneClientSession::OnConnected()
{
    // TODO: 로그인 처리 또는 초기화 요청
}

void ZoneClientSession::OnDisconnected()
{
    _manager->Remove(_sessionId);
}
