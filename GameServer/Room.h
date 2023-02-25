#pragma once
#include "Proto/Protocol.pb.h"

#include "JobQueue.h"
#include "GameMap.h"
class Room : public JobQueue
{
public:
	Room() = delete;
	Room(uint64 roomId, GameMap&& map, uint64 hostAidx, uint32 maxMemberCount, uint32 minMemberCount);

	~Room() = default;

	bool Init();
	void Update();

	RoomRef			GetRoomRef() { return static_pointer_cast<Room>(shared_from_this()); }

	void Enter(UserRef user);
	void Leave(UserRef user);
	void Ping(UserRef user);

	void Spawn(ActorRef actor);
	void Despawn(ActorRef actor);
	void Broadcast(uint16 packetId, google::protobuf::MessageLite& packet);
	void Broadcast(uint16 packetId, google::protobuf::MessageLite& packet, uint64 ignore);



	void ApplyAction(GameSessionRef session, PlayerRef player, Protocol::C_ACTION pkt);
	void ApplyPlayerBomb(PlayerRef bombOwner);
	void ApplyExplodeBomb(uint64 bombId);
	bool IsCollision(ActorRef actor, Position dest);

	std::atomic<uint64> actorId = 1;

private:
	std::map<uint64, ActorRef> mActorMap;
	std::map<uint64, UserRef> mUserMap;
	GameMap mGameMap;
	uint64 mId;
	uint64 mHostAidx;
	uint32 mMaxMemberCount;
	uint32 mMinMemberCount;

	ActorRef findActor(uint64 id);
};