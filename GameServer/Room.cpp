#include "stdafx.h"
#include "Room.h"
#include "Actor.h"
#include "Player.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "Block.h"
#include "User.h"
#include "Bomb.h"

Room::Room(uint64 roomId, GameMap&& map, uint64 hostAidx, uint32 maxMemberCount, uint32 minMemberCount)
	: mId(roomId),
	mGameMap(map),
	mHostAidx(hostAidx),
	mMaxMemberCount(maxMemberCount),
	mMinMemberCount(mMinMemberCount)
{
}

bool Room::Init()
{
	if (mGameMap.Init(GetRoomRef()) == false)
		return false;

	DoTimer(50, &Room::ViewUpdate);

	return true;
}

void Room::Update()
{
}

void Room::ViewUpdate()
{
	DoTimer(50, &Room::ViewUpdate);

	for (auto [aidx, userRef] : mUserMap)
	{
		auto session = userRef->GetSession();
		if (session == nullptr)
			continue;

		if (userRef->mPlayer == nullptr)
			continue;

		userRef->mPlayer->GetView().Update(session);
	}
}


void Room::Enter(UserRef user)
{
	auto playerRef = user->mPlayer;
	playerRef->SetId(actorId);

	//원래 있던 객체 보내주기
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_myplayerid(actorId);
		

		for (auto [id, actor] : mActorMap)
		{
			auto spawnActor = enterGamePkt.add_exisitingactors();
			actor->SetActorInfo(spawnActor);
		}

		if (auto session = user->GetSession(); session == nullptr)
			return;
		else
		{
			session->Send(1, enterGamePkt);
		}
	}


	mActorMap[actorId] = playerRef;
	mUserMap[user->mAidx] = user;

	actorId++;
	
	Ping(user);
}


void Room::Leave(UserRef user)
{
	mActorMap.erase(user->mPlayer->mId);
	mUserMap.erase(user->mAidx);

	user->mPlayer->GetView().Destroy();
	user->mPlayer = nullptr;
}

void Room::Ping(UserRef user)
{
	if (auto session = user->GetSession(); session == nullptr || session->mConnected == false)
		return;
	else
	{
		if (user->PingCheck() == false)
		{
			session->DisConnect("No HeartBeat Session Disconnect ! ");
			return;
		}

		//cout << "SEND PING " << endl;
		Protocol::S_PING pingPkt;
		session->Send(0, pingPkt);

		DoTimer(1000, &Room::Ping, user);
	}

}

void Room::Spawn(ActorRef actor)
{
	if(mActorMap.find(actor->mId) == mActorMap.end())
		mActorMap[actor->mId] = actor;

	//Protocol::S_SPAWN spawnPkt;

	//auto spawnActor = spawnPkt.add_actors();
	//actor->SetActorInfo(spawnActor);

	//cout << "SPAWN : " << actor->mId << endl;

	//BroadcastNear(actor->mPos, 2, spawnPkt);
}

void Room::Despawn(ActorRef actor)
{
	if (mActorMap.find(actor->mId) != mActorMap.end())
		mActorMap.erase(actor->mId);


	//Protocol::S_DESPAWN despawnPkt;

	//auto despawnActor = despawnPkt.add_actors();
	//actor->SetActorInfo(despawnActor);

	//BroadcastNear(actor->mPos, 3, despawnPkt);
}

void Room::Broadcast(uint16 packetId, google::protobuf::MessageLite& packet)
{
	for (auto [aidx, userRef] : mUserMap)
	{
		auto session = userRef->GetSession();
		if (session == nullptr) // weak 처리
			continue;

		if(userRef->mPlayer == nullptr)
			continue;
		
		session->Send(packetId, packet);
	}
}

void Room::Broadcast(uint16 packetId, google::protobuf::MessageLite& packet, uint64 ignore)
{
	for (auto [aidx, userRef] : mUserMap)
	{
		if (aidx == ignore) continue;
		if (auto session = userRef->GetSession(); session == nullptr)
			continue;
		else
		{
			session->Send(packetId, packet);
		}

	}
}

void Room::BroadcastNear(Position src, uint16 packetId, google::protobuf::MessageLite& packet)
{
	auto nearUser = GetNearUsers(src);

	for (auto userRef : nearUser)
	{
		auto session = userRef->GetSession();
		if (session == nullptr) continue;

		session->Send(packetId, packet);
	}

	nearUser.clear();
}

std::set<ActorRef> Room::GetNearActors(Position src)
{
	// 최적화 필요
	std::set<ActorRef> actors;

	for (auto [id, actorRef] : mActorMap)
	{
		auto [targetX, targetY] = actorRef->mPos;

		float fDistanceX = fabsf(src.x - targetX);
		float fRadCX = 500.f; // CONFIG

		float fDistanceY = fabsf(src.y - targetY);
		float fRadCY = 500.f; // CONFIG

		if (fDistanceX < fRadCX && fDistanceY < fRadCY)
			actors.insert(actorRef);

	}

	return actors;
}

std::set<UserRef> Room::GetNearUsers(Position src)
{
	// 최적화 필요
	std::set<UserRef> users;

	for (auto [aidx, userRef] : mUserMap)
	{
		if (userRef->mPlayer == nullptr) 
			continue;

		auto [targetX, targetY] = userRef->mPlayer->mPos;

		float fDistanceX = fabsf(src.x - targetX);
		float fRadCX = 500.f; // CONFIG

		float fDistanceY = fabsf(src.y - targetY);
		float fRadCY = 500.f; // CONFIG

		if (fDistanceX < fRadCX && fDistanceY < fRadCY)
			users.insert(userRef);
	}

	return users;
}

void Room::ApplyAction(GameSessionRef session, PlayerRef player, Protocol::C_ACTION pkt)
{
	if (pkt.playeractions_size() <= 0) return;
	if(player->mState == State::DIE) return;

	auto actorSpeed = player->GetSpeed();

	auto actions = pkt.playeractions();
	for (auto action : actions)
	{
		float hopeX = player->mPos.x;
		float hopeY = player->mPos.y;

		switch (action)
		{
		case Protocol::Action::ACTION_DOWN:
			hopeY += actorSpeed * 1.f;
			break;
		case Protocol::Action::ACTION_UP:
			hopeY -= actorSpeed * 1.f;
			break;
		case Protocol::Action::ACTION_LEFT:
			hopeX -= actorSpeed * 1.f;
			break;
		case Protocol::Action::ACTION_RIGHT:
			hopeX += actorSpeed * 1.f;
			break;
		case Protocol::Action::ACTION_ATTACK:	
		{
			ApplyPlayerBomb(player);
			return;
		}
			break;
		default:
			break;
		}

		if (mGameMap.ApplyMove(player, { hopeX, hopeY }) == false)
			return;



		Protocol::S_ACTION  actionPkt;
		actionPkt.set_tickcount(GetTickCount64());
		actionPkt.set_actorid(player->mId);
		actionPkt.set_playeraction(static_cast<Protocol::Action>(action));

		BroadcastNear(player->mPos, 4, actionPkt);
	}
}


void Room::ApplyPlayerBomb(PlayerRef bombOwner)
{
	if (bombOwner == nullptr)
		return;
	Position pos = mGameMap.SearchMapPosition(bombOwner->mPos);

	if (mGameMap.IsOccupied(pos.x/32, pos.y/32)) return;	

	BombRef bombRef = MakeShared<Bomb>(bombOwner);
	bombRef->mType = Protocol::ActorType::ACTOR_TYPE_BOMB;
	
	bombRef->SetPosition({ pos.x, pos.y });
	bombRef->mId = actorId++;
	Spawn(bombRef);


	SetOnPlaceUsers(bombRef);
	mActorMap[bombRef->mId] = bombRef;
	mGameMap.Occupy(pos.x/32, pos.y/32);

	DoTimer(3000, [pos, id = bombRef->mId, room = bombOwner->GetRoom()]() {
		if (room == nullptr) return;
		
		auto bomb = room->findActor(id);
		if (bomb == nullptr) return;

		room->ApplyExplodeBomb(id);

		room->Despawn(bomb);
		room->GetMap().Away(static_cast<short>(pos.x/32), static_cast<short>(pos.y/32));
	});
}


void Room::ApplyExplodeBomb(uint64 bombId)
{
	auto actor = findActor(bombId);
	if (actor == nullptr) return;
	if (actor->mType != Protocol::ACTOR_TYPE_BOMB)
		return; // CRASH도 가능

	auto bomb = static_pointer_cast<Bomb>(actor);
	if (bomb)
	{
		auto [x, y] = bomb->mPos;

		Vector<ActorRef> toLeaves;
		for (auto [id, mapActor] : mActorMap)
		{
			if (id != bomb->mId && bomb->IsInRange(mapActor))
			{
				toLeaves.push_back(mapActor);
			}
		}

		for (auto leaveActor : toLeaves)
		{
			if (leaveActor->mType == Protocol::ACTOR_TYPE_PLAYER)
			{
				Protocol::S_ACTION  actionPkt;
				actionPkt.set_tickcount(GetTickCount64());
				actionPkt.set_actorid(leaveActor->mId);
				actionPkt.set_playeraction(Protocol::Action::ACTION_TEMP_DIE);

				leaveActor->mState = State::DIE;

				BroadcastNear(leaveActor->mPos, 4, actionPkt);
				
				DoTimer(5000, &Room::CheckDie, leaveActor);
				continue;
			}

			auto pos = mGameMap.SearchMapPosition(leaveActor->mPos);
			if (mGameMap.IsOccupied(pos.x/32, pos.y/32)) mGameMap.Away(pos.x/32, pos.y/32);

			DoTimer(60, &Room::Despawn, leaveActor);
		}
		

		toLeaves.clear();
	}

	mActorMap.erase(bombId);
}


bool Room::IsCollision(ActorRef actor, Position dest)
{
	auto myId = actor->mId;
	for (auto [id, mapActor] : mActorMap)
	{
		if (id == 0) continue;
		if (id == myId) continue;
		if (mapActor->mType == Protocol::ACTOR_TYPE_PLAYER)
		{
			if (mapActor->mState == State::DIE)
			{
				mapActor->mState = State::LIVE;
			}
		}


		auto [x, y] = mapActor->mPos;
		auto [destX, destY] = dest;
		//나중에 Position 구조체안에 오버로딩

		float fDistanceX = fabsf(x - destX);
		float fRadCX = (mapActor->mWidthSize + actor->mWidthSize) / 2;

		float fDistanceY = fabsf(y - destY);
		float fRadCY = (mapActor->mHeightSize + actor->mHeightSize) / 2;

		bool collision = false;
		if (fDistanceX < fRadCX && fDistanceY < fRadCY)
		{
			collision = true;

			if (mapActor->mType != Protocol::ACTOR_TYPE_BOMB)
				return true;
		}

		if (mapActor->mType == Protocol::ACTOR_TYPE_BOMB)
		{
			auto bombRef = static_pointer_cast<Bomb>(mapActor);

			if (actor->mType == Protocol::ACTOR_TYPE_PLAYER)
			{
				auto player = static_pointer_cast<Player>(actor);
				if (bombRef->OnUserIds.size() == 0 && collision == true) return true;

				if (collision == false && bombRef->OnUserIds.find(player->mId) != bombRef->OnUserIds.end())
				{
					bombRef->OnUserIds.erase(player->mId);
				}
			}
		}
	}

	return false;
}

void Room::CheckDie(ActorRef actor)
{
	if (actor->mState == State::DIE)
	{
		Despawn(actor);

		//테스트용
		//actor->mState = State::LIVE;
		//Protocol::S_ACTION  actionPkt;
		//actionPkt.set_tickcount(GetTickCount64());
		//actionPkt.set_actorid(actor->mId);
		//actionPkt.set_playeraction(Protocol::Action::ACTION_RESURRECT);

		//BroadcastNear(actor->mPos, 4, actionPkt);
	}
	
	else if (actor->mState == State::LIVE)
	{
		Protocol::S_ACTION  actionPkt;
		actionPkt.set_tickcount(GetTickCount64());
		actionPkt.set_actorid(actor->mId);
		actionPkt.set_playeraction(Protocol::Action::ACTION_RESURRECT);

		BroadcastNear(actor->mPos, 4, actionPkt);
	}
}

void Room::SetOnPlaceUsers(BombRef bomb)
{
	Vector<weak_ptr<Player>> ret;

	for (auto [id, user] : mUserMap)
	{
		auto player = user->mPlayer;
		
		auto [x, y] = player->mPos;
		auto [destX, destY] = bomb->mPos;
		//나중에 Position 구조체안에 오버로딩

		float fDistanceX = fabsf(x - destX);
		float fRadCX = (player->mWidthSize + bomb->mWidthSize) / 2;

		float fDistanceY = fabsf(y - destY);
		float fRadCY = (player->mHeightSize + bomb->mHeightSize) / 2;

		if (fDistanceX < fRadCX && fDistanceY < fRadCY)
		{
			bomb->OnUserIds.insert(player->mId);
		}	
	}
}

void Room::Test(GameSessionRef session)
{
	DoTimer(1000, &Room::Test, session);
	// 기존 패킷 활용

	Protocol::S_ENTER_GAME enterGamePkt;

	for (auto [id, actor] : mActorMap)
	{
		auto spawnActor = enterGamePkt.add_exisitingactors();
		actor->SetActorInfo(spawnActor);
	}

	session->Send(1, enterGamePkt);
}


ActorRef Room::findActor(uint64 id)
{
	if (mActorMap.find(id) == mActorMap.end())
		return nullptr;
	return mActorMap[id];
}