#include "stdafx.h"
#include "Room.h"
#include "Actor.h"
#include "Player.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "Block.h"
#include "User.h"

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

	DoTimer(100, &Room::ViewUpdate);

	return true;
}

void Room::Update()
{
}

void Room::ViewUpdate()
{
	//cout << "View Update" << endl;

	DoTimer(100, &Room::ViewUpdate);

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

	Protocol::S_SPAWN spawnPkt;

	auto spawnActor = spawnPkt.add_actors();
	actor->SetActorInfo(spawnActor);

	Broadcast(2, spawnPkt);
	//BroadcastNear(actor->mPos, 2, spawnPkt);
}

void Room::Despawn(ActorRef actor)
{
	Protocol::S_DESPAWN despawnPkt;

	auto despawnActor = despawnPkt.add_actors();
	actor->SetActorInfo(despawnActor);
	cout << "DESPAWN " << actor->mId << endl;

	Broadcast(3, despawnPkt);
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
	std::set<ActorRef> actors;

	for (auto [id, actorRef] : mActorMap)
	{
		auto [targetX, targetY] = actorRef->mPos;

		float fDistanceX = fabsf(src.x - targetX);
		float fRadCX = 100.f;

		float fDistanceY = fabsf(src.y - targetY);
		float fRadCY = 100.f;

		if (fDistanceX < fRadCX && fDistanceY < fRadCY)
			actors.insert(actorRef);

	}

	return actors;
}

std::set<UserRef> Room::GetNearUsers(Position src)
{
	std::set<UserRef> users;

	for (auto [aidx, userRef] : mUserMap)
	{
		if (userRef->mPlayer == nullptr) 
			continue;

		auto [targetX, targetY] = userRef->mPlayer->mPos;

		float fDistanceX = fabsf(src.x - targetX);
		float fRadCX = 100.f;

		float fDistanceY = fabsf(src.y - targetY);
		float fRadCY = 100.f;

		if (fDistanceX < fRadCX && fDistanceY < fRadCY)
			users.insert(userRef);
	}

	return users;
}

void Room::ApplyAction(GameSessionRef session, PlayerRef player, Protocol::C_ACTION pkt)
{
	if (pkt.playeractions_size() <= 0) return;
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
		default:
			break;
		}

		if (mGameMap.ApplyMove(player, { hopeX, hopeY }) == false)
			return;



		Protocol::S_ACTION  actionPkt;
		actionPkt.set_tickcount(GetTickCount64());
		actionPkt.set_actorid(player->mId);
		actionPkt.set_playeraction(static_cast<Protocol::Action>(action));

		//Broadcast(4, actionPkt);
		BroadcastNear(player->mPos, 4, actionPkt);
	}
}


void Room::ApplyPlayerBomb(PlayerRef bombOwner)
{
	//if (bombOwner == nullptr)
	//	return;



	//ActorRef actor = MakeShared<Bomb>(bombOwner->mOwnerSession);
	////actor->mName = u8"폭탄";
	//actor->mType = Protocol::ActorType::ACTOR_TYPE_BOMB;
	//auto startPos = bombOwner->SearchBombPosition();
	//actor->SetStartPosition(startPos, false);

	//actor->Spawn(GetRoomRef());

	//mActorMap[actor->mId] = actor;

	//std::cout << "플레이어 (" << bombOwner->mId << ")가 (" << actor->mId << ")로 공격" << std::endl;

	//{
	//	Protocol::S_SPAWN spawnPkt;

	//	auto spawnActor = spawnPkt.add_actor();
	//	spawnActor->set_id(actor->mId);
	//	//spawnActor->set_name(actor->mName);
	//	spawnActor->set_type(actor->mType);
	//	actor->SetDetailType(spawnActor);

	//	auto pos = spawnActor->mutable_position();
	//	pos->set_x(actor->mPos.first);
	//	pos->set_y(actor->mPos.second);

	//	auto spawnPacketBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);

	//	Broadcast(spawnPacketBuffer);

	//	DoTimer(3000, [id = actor->mId, room = actor->mRoom]() {
	//		if (auto roomRef = room.lock())
	//		{
	//			auto actor = roomRef->findActor(id);
	//			if (actor)
	//			{
	//				Protocol::S_DESPAWN broadCastPkt;

	//				auto despawnnActor = broadCastPkt.add_actor();
	//				despawnnActor->set_id(actor->mId);

	//				auto broadCastBuffer = ClientPacketHandler::MakeSendBuffer(broadCastPkt);
	//				roomRef->Broadcast(broadCastBuffer);
	//			}
	//		}
	//	});
	//}
}


void Room::ApplyExplodeBomb(uint64 bombId)
{
	//auto actor = findActor(bombId);
	//auto bomb = dynamic_pointer_cast<Bomb>(actor);

	//if (bomb)
	//{
	//	auto [x, y] = bomb->mPos;

	//	Vector<ActorRef> toLeaves;
	//	for (auto [id, mapActor] : mActorMap)
	//	{
	//		if (id != bomb->mId && bomb->IsInRange(mapActor))
	//		{
	//			toLeaves.push_back(mapActor);
	//		}
	//	}

	//	for (auto leaveActor : toLeaves)
	//		Leave(leaveActor);

	//	toLeaves.clear();
	//}

	//mActorMap.erase(bombId);
}


bool Room::IsCollision(ActorRef actor, Position dest)
{
	auto myId = actor->mId;
	for (auto [id, mapActor] : mActorMap)
	{
		if (id == 0) continue;
		if (id == myId) continue;
		if (mapActor->mType == Protocol::ACTOR_TYPE_PLAYER) continue;
		if (mapActor->mType == Protocol::ACTOR_TYPE_BOMB) continue;

		auto [x, y] = mapActor->mPos;
		auto [destX, destY] = dest;
		//나중에 Position 구조체안에 오버로딩

		float fDistanceX = fabsf(x - destX);
		float fRadCX = (mapActor->mWidthSize + actor->mWidthSize) / 2;

		float fDistanceY = fabsf(y - destY);
		float fRadCY = (mapActor->mHeightSize + actor->mHeightSize) / 2;

		if (fDistanceX < fRadCX && fDistanceY < fRadCY)
			return true;
	}

	return false;
}


ActorRef Room::findActor(uint64 id)
{
	if (mActorMap.find(id) == mActorMap.end())
		return nullptr;
	return mActorMap[id];
}