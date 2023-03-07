#pragma once
#include "Proto/Protocol.pb.h"

#include "JobQueue.h"
#include "GameMap.h"
class Room : public JobQueue
{
public:
	Room() = delete;
	Room(uint64 roomId, GameMap&& map, uint64 hostAidx, uint32 maxMemberCount, uint32 minMemberCount, float viewSize, uint32 viewDelay);

	~Room() {
		cout << "~Room" << endl;
	};

	bool Init();
	void Update();
	void ViewUpdate();
	void PreClose();
	void Close();

	RoomRef			GetRoomRef() { return static_pointer_cast<Room>(shared_from_this()); }
	GameMap& GetMap() { return mGameMap; }
	float			GetViewSize() const { return mViewSize; }

	void Enter(UserRef user);
	void Leave(UserRef user);
	void Ping(UserRef user);

	void Spawn(ActorRef actor);
	void Despawn(ActorRef actor);
	void Broadcast(uint16 packetId, google::protobuf::MessageLite& packet);
	void Broadcast(uint16 packetId, google::protobuf::MessageLite& packet, uint64 ignore);
	void BroadcastNear(Position pos, uint16 packetId, google::protobuf::MessageLite& packet);


	std::set<ActorRef> GetNearActors(Position src);
	std::set<UserRef> GetNearUsers(Position src);

	void ApplyAction(GameSessionRef session, PlayerRef player, Protocol::C_ACTION pkt);
	void ApplyPlayerBomb(PlayerRef bombOwner);
	void ApplyExplodeBomb(uint64 bombId);

	bool IsCollision(ActorRef actor, Position dest);


	void CheckDie(ActorRef actor);
	void SetOnPlaceUsers(BombRef bomb);

	std::atomic<uint64> actorId = 1;

	void Test(GameSessionRef session);

	Set<uint64> mPlayers;


private:
	Map<uint64, ActorRef> mActorMap;
	Map<uint64, UserRef> mUserMap;
	GameMap mGameMap;
	uint64 mId;
	uint64 mHostAidx;
	uint32 mMaxMemberCount;
	uint32 mMinMemberCount;
	uint32	mViewDelay;
	float	mViewSize = 0.f;
	bool	mStart = false;
	bool	mClose = false;

	

	ActorRef findActor(uint64 id);
};