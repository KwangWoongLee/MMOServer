#include "stdafx.h"
#include "RoomManager.h"

#include "Room.h"
#include "Actor.h"
#include "Player.h"
#include "GameSession.h"
#include "User.h"

std::atomic<uint64_t> RoomManager::smRoomId = 1;
shared_ptr<RoomManager> gRoomManager = make_shared<RoomManager>();

uint64_t RoomManager::Add(uint64_t hostAidx, uint32_t gameMapId, uint32_t maxMemberCount, uint32_t minMemberCount, float viewSize, uint32_t viewDelay)
{
	uint64_t roomId = smRoomId++;
	auto roomRef = MakeShared<Room>(roomId, GameMap(gameMapId), hostAidx, maxMemberCount, minMemberCount, viewSize, viewDelay);
	if (roomRef->Init() == false)
	{
		roomRef = nullptr;
		return 0;
	}
		

	{
		WRITE_LOCK;
		mId2Room[roomId] = roomRef;
	}

	return roomId;
	
}

void RoomManager::Remove(uint64_t roomId)
{
	WRITE_LOCK;
	auto iter = mId2Room.find(roomId);
	if (iter == mId2Room.end())
		return;

	mId2Room.erase(iter);
}


RoomRef RoomManager::Find(uint64_t roomId)
{
	WRITE_LOCK;
	auto iter = mId2Room.find(roomId);
	if (iter == mId2Room.end())
		return nullptr;

	return iter->second;
}

void RoomManager::Update()
{
	WRITE_LOCK;
	for (auto [id, roomRef] : mId2Room)
	{
		roomRef->DoAsync(&Room::Update);
	}
}
