#pragma once

class GameSession;

using GameSessionRef = shared_ptr<GameSession>;

class GameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(uint16 packetId, google::protobuf::MessageLite& packet);

private:
	USE_LOCK;
	std::set<GameSessionRef> mSessions;
};

extern GameSessionManager gSessionManager;
