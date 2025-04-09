#pragma once
#include "Proto/Protocol.pb.h"

#include "JobQueue.h"
#include "GameMap.h"
class Room : public JobQueue
{
public:
	Room() = delete;
	Room(uint64_t roomId, GameMap&& map, uint64_t hostAidx, uint32_t maxMemberCount, uint32_t minMemberCount, float viewSize, uint32_t viewDelay);

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
	void Broadcast(uint16_t packetId, google::protobuf::MessageLite& packet);
	void Broadcast(uint16_t packetId, google::protobuf::MessageLite& packet, uint64_t ignore);
	void BroadcastNear(Position pos, uint16_t packetId, google::protobuf::MessageLite& packet);


	std::set<ActorRef> GetNearActors(Position src);
	std::set<UserRef> GetNearUsers(Position src);

	Vector<ZoneRef> GetNearZones(Position src);

	void ApplyAction(GameSessionRef session, PlayerRef player, Protocol::C_ACTION pkt);
	void ApplyPlayerBomb(PlayerRef bombOwner);
	void ApplyExplodeBomb(uint64_t bombId);

	bool IsCollision(ActorRef actor, Position dest);
	ZoneRef GetZone(Position pos);
	std::pair<short, short> GetZoneIndexByPosition(Position pos);
	std::pair<short, short> GetZoneIndexByMapIndex(short mapX, short mapY);


	void CheckDie(ActorRef actor);
	void SetOnPlaceUsers(BombRef bomb);

	std::atomic<uint64_t> actorId = 1;

	void Test(GameSessionRef session);

	Set<uint64_t> mPlayers;


private:
	Map<uint64_t, ActorRef> mActorMap;
	Map<uint64_t, UserRef> mUserMap;
	Vector<Vector<ZoneRef>> mZones;
	uint32_t  mZoneSize;
	GameMap mGameMap;
	uint64_t mId;
	uint64_t mHostAidx;
	uint32_t mMaxMemberCount;
	uint32_t mMinMemberCount;
	uint32_t	mViewDelay;
	float	mViewSize = 0.f;
	bool	mStart = false;
	bool	mClose = false;




	ActorRef findActor(uint64_t id);
};