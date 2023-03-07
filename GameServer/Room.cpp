#include "stdafx.h"
#include "Room.h"
#include "Actor.h"
#include "Player.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "Block.h"
#include "User.h"
#include "Bomb.h"
#include "RoomManager.h"
#include "RedisManager.h"

Room::Room(uint64 roomId, GameMap&& map, uint64 hostAidx, uint32 maxMemberCount, uint32 minMemberCount, float viewSize, uint32 viewDelay)
	: mId(roomId),
	mGameMap(map),
	mHostAidx(hostAidx),
	mMaxMemberCount(maxMemberCount),
	mMinMemberCount(mMinMemberCount),
	mViewSize(viewSize),
	mViewDelay(viewDelay)
{
}

bool Room::Init()
{
	if (mGameMap.Init(GetRoomRef()) == false)
		return false;

	DoTimer(mViewDelay, &Room::ViewUpdate);

	return true;
}

void Room::Update()
{
#ifndef TEST
	//if (mClose == true) return;

	//if (mStart == true && mPlayers.size() == 0)
	//	PreClose();
#endif // !TEST

}

void Room::ViewUpdate()
{
	if (mClose == true) return;

	DoTimer(mViewDelay, &Room::ViewUpdate);

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

void Room::Close()
{

	// 반복자 무효화 -> 이렇게 하면 안됨
	//for (auto [aidx, userRef] : mUserMap)
	//{
	//	auto session = userRef->GetSession();
	//	if (session == nullptr)
	//		continue;

	//	if (userRef->mPlayer == nullptr)
	//		continue;

	//	Leave(userRef);
	//}


	// 공부할 때 한번 더 볼 것 , 연관 컨테이너에서의 반복자 무효화 및 erase
	auto iter = mUserMap.begin();
	for (; iter != mUserMap.end();)
	{
		auto userRef = (*iter).second;
		auto session = userRef->GetSession();
		if (session == nullptr)
			++iter;

		if (userRef->mPlayer == nullptr)
			++iter;

		Leave((*(iter++)).second);
	}


	mActorMap.clear();

	gRoomManager->Remove(mId);
}


void Room::Enter(UserRef user)
{
	auto playerRef = user->mPlayer;
	playerRef->SetId(actorId);

	//원래 있던 객체 보내주기
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_myplayerid(actorId);
		
		if (auto session = user->GetSession(); session == nullptr)
			return;
		else
		{
			session->Send(1, enterGamePkt);
		}
	}


	mActorMap[actorId] = playerRef;
	mUserMap[user->mAidx] = user;
	mPlayers.insert(actorId);
	
	mStart = true;
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

		Protocol::S_PING pingPkt;
		session->Send(0, pingPkt);

		DoTimer(1000, &Room::Ping, user);
	}

}

void Room::Spawn(ActorRef actor)
{
	if(mActorMap.find(actor->mId) == mActorMap.end())
		mActorMap[actor->mId] = actor;
}

void Room::Despawn(ActorRef actor)
{
	if (mActorMap.find(actor->mId) != mActorMap.end())
		mActorMap.erase(actor->mId);
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
		float fRadCX = mViewSize; // CONFIG

		float fDistanceY = fabsf(src.y - targetY);
		float fRadCY = mViewSize; // CONFIG

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
		float fRadCX = mViewSize; // CONFIG

		float fDistanceY = fabsf(src.y - targetY);
		float fRadCY = mViewSize; // CONFIG

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


	// 물폭탄 설치 후 3초 후 터짐 효과, 서버에서는 액터를 굳이 생성 X
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
	auto [x, y] = bomb->mPos;

	Vector<ActorRef> toLeaves;
	for (auto [id, mapActor] : mActorMap)
	{
		if (id != 0 && id != bomb->mId && bomb->IsInRange(mapActor))
		{
			toLeaves.push_back(mapActor);
		}
	}

	auto owner = bomb->GetOwner();


	for (auto leaveActor : toLeaves)
	{
		if (leaveActor->mType == Protocol::ACTOR_TYPE_BLOCK && owner)
			owner->mKillCount += 1;


		if (leaveActor->mType == Protocol::ACTOR_TYPE_PLAYER)
		{
			cout << leaveActor->mId << endl;

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
		if (mGameMap.IsOccupied(pos.x / 32, pos.y / 32)) mGameMap.Away(pos.x / 32, pos.y / 32);

		DoTimer(60, &Room::Despawn, leaveActor);
	}


	toLeaves.clear();

	mActorMap.erase(bombId);
}


bool Room::IsCollision(ActorRef actor, Position dest)
{
	auto myId = actor->mId;
	for (auto [id, mapActor] : mActorMap)
	{
		if (id == 0) continue;
		if (id == myId) continue;


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
			if (mapActor->mType == Protocol::ACTOR_TYPE_PLAYER)
			{
				if (mapActor->mState == State::DIE)
				{
					mapActor->mState = State::LIVE;
				}
			}

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
	if (actor->mType != Protocol::ACTOR_TYPE_PLAYER)
		return; // CRASH

	if (actor->mState == State::DIE)
	{
#ifdef TEST
		//테스트용
		//actor->mState = State::LIVE;
		//Protocol::S_ACTION  actionPkt;
		//actionPkt.set_tickcount(GetTickCount64());
		//actionPkt.set_actorid(actor->mId);
		//actionPkt.set_playeraction(Protocol::Action::ACTION_RESURRECT);

		//BroadcastNear(actor->mPos, 4, actionPkt);
#else
		//플레이어 일 때만 들어옴
		Despawn(actor);

		mPlayers.erase(actor->mId);
		if (mPlayers.size() == 0) // 게임종료
		{
			PreClose();
		}
#endif // TEST

		
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

void Room::PreClose()
{
	if (mClose == true) return;
	mClose = true;

	for (const auto& [aidx, userRef] : mUserMap)
	{
		auto session = userRef->GetSession();
		if (session == nullptr) continue;

		Protocol::S_LEAVE_GAME  leavePkt;
		session->Send(5, leavePkt);
	}

	//// 종료되었다면 레디스에 게임 결과 업데이트
	std::string query = "";
	query += "set room";
	//query += to_string(mId);
	query += " 1";

	gRedisManager->DoAsync(&RedisManager::Exec, query);

	// 0.9초 후 게임 종료
	DoTimer(900, &Room::Close);
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
	DoTimer(200, &Room::Test, session);

	Protocol::S_TEST testPkt;

	for (auto [id, actorRef] : mActorMap)
	{
		auto actor = testPkt.add_actors();
		actorRef->SetActorInfo(actor);
	}

	session->Send(1, testPkt);
}


ActorRef Room::findActor(uint64 id)
{
	if (mActorMap.find(id) == mActorMap.end())
		return nullptr;
	return mActorMap[id];
}