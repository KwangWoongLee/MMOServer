#include "stdafx.h"
#include "RoomManager.h"

#include "Room.h"
#include "Actor.h"
#include "Player.h"
#include "GameSession.h"
#include "User.h"

std::atomic<uint64> RoomManager::smRoomId = 1;
shared_ptr<RoomManager> gRoomManager = make_shared<RoomManager>();

uint64 RoomManager::Add(uint64 hostAidx, uint32 gameMapId, uint32 maxMemberCount, uint32 minMemberCount)
{
	uint64 roomId = smRoomId++;
	auto roomRef = std::make_shared<Room>(roomId, GameMap(gameMapId), hostAidx, maxMemberCount, minMemberCount);
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

void RoomManager::Remove(uint64 roomId)
{
	WRITE_LOCK;
	auto iter = mId2Room.find(roomId);
	if (iter == mId2Room.end())
		return;

	mId2Room.erase(iter);
}


RoomRef RoomManager::Find(uint64 roomId)
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
