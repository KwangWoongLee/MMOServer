#include "stdafx.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "Proto/protocol.pb.h"

GameSessionManager gSessionManager;

void GameSessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	mSessions.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	mSessions.erase(session);
}

void GameSessionManager::Broadcast(uint16_t packetId, google::protobuf::MessageLite& packet)
{
	WRITE_LOCK;
	for (GameSessionRef session : mSessions)
	{
		session->Send(packetId, packet);
	}
}