#include "stdafx.h"
#include "ZoneWorldSession.h"
#include "ZoneWorldSessionManager.h"

ZoneWorldSession::ZoneWorldSession(ZoneWorldSessionManager* manager)
    : _manager(manager)
{
}

void ZoneWorldSession::OnConnected()
{
    // TODO: 월드 서버에 접속 완료 후 등록 패킷 보내기
}

void ZoneWorldSession::OnDisconnected()
{
    _manager->Remove(_sessionId);
}