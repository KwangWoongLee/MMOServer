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
	return mGameMap.Init(GetRoomRef());
}

void Room::Update()
{

}

void Room::Enter(UserRef user)
{
	actorId++;
	auto playerRef = user->mPlayer;

	playerRef->Spawn(GetRoomRef());

	//원래 있던 객체 보내주기
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_myplayerid(playerRef->mId);
		

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

	mUserMap[user->mAidx] = user;
	
	Spawn(playerRef);

	Ping(user);
}


void Room::Leave(UserRef user)
{
	Despawn(user->mPlayer);
	user->mPlayer = nullptr;

	mUserMap.erase(user->mAidx);

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
	mActorMap[actor->mId] = actor;

	Protocol::S_SPAWN spawnPkt;

	auto spawnActor = spawnPkt.add_actors();
	actor->SetActorInfo(spawnActor);

	Broadcast(2, spawnPkt);
}

void Room::Despawn(ActorRef actor)
{
	Protocol::S_DESPAWN despawnPkt;

	auto despawnActor = despawnPkt.add_actors();
	actor->SetActorInfo(despawnActor);
	cout << "DESPAWN " << actor->mId << endl;

	Broadcast(3, despawnPkt);

	mActorMap.erase(actor->mId);
}

void Room::Broadcast(uint16 packetId, google::protobuf::MessageLite& packet)
{
	for (auto [aidx, userRef] : mUserMap)
	{
		auto session = userRef->GetSession();
		if (session == nullptr) // weak 처리
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



		Protocol::S_ACTION  broadCastPkt;
		broadCastPkt.set_tickcount(GetTickCount64());
		broadCastPkt.set_actorid(player->mId);
		broadCastPkt.set_playeraction(static_cast<Protocol::Action>(action));
		
		//Broadcast(4, broadCastPkt, session->GetUser()->mAidx);
		Broadcast(4, broadCastPkt);
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