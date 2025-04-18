#include "stdafx.h"
#include "GameMap.h"
#include "ConfigManager.h"
#include "Actor.h"
#include "Room.h"
#include "Block.h"
#include "Player.h"
#include "Zone.h"

#include <fstream>
#include <sstream>

//#include "Block.h"

GameMap::GameMap(uint8_t id)
	: mId(id)
{
}

#pragma region 이동
bool GameMap::ApplyMove(ActorRef actor, Position dest)
{
	if (actor == nullptr)
		return false;

	if (canGo(actor, dest) == false)
		return false;


	// Zone Add
	auto roomRef = mRoom.lock();
	if (roomRef == nullptr) return false;

	auto srcZone = roomRef->GetZone(actor->mPos);
	auto destZone = roomRef->GetZone(dest);
	if (srcZone != destZone)
	{
		srcZone->RemoveActor(actor);
		destZone->AddActor(actor);

		if (actor->mType == Protocol::ACTOR_TYPE_PLAYER)
		{
			auto playerRef = static_pointer_cast<Player>(actor);

			auto userRef = playerRef->GetUser();
			if (userRef == nullptr) return false;

			srcZone->RemoveUser(userRef);
			destZone->AddUser(userRef);
		}
	}

	actor->mPos = dest;
	return true;
}

void GameMap::Occupy(short x, short y)
{
	mMapCollision[x][y] = 1;
}

void GameMap::Away(short x, short y)
{
	mMapCollision[x][y] = 0;
}

bool GameMap::IsOccupied(short x, short y)
{
	return mMapCollision[x][y] == 1;
}

Position GameMap::SearchMapPosition(Position pos)
{	
	auto [x, y] = pos;
	auto roundX = std::lroundf(x);
	auto roundY = std::lroundf(y);

	int retX;
	int retY;
	if (roundX % 32 > 16)
		retX = ((roundX / 32) + 1) * 32;
	else
		retX = (roundX / 32) * 32;

	if (roundY % 32 > 16)
		retY = ((roundY / 32) + 1) * 32;
	else
		retY = (roundY / 32) * 32;

	return { static_cast<float>(retX), static_cast<float>(retY) };
}

std::pair<short, short> GameMap::SearchMapIndex(Position pos)
{
	auto [x, y] = pos;
	auto roundX = std::lroundf(x);
	auto roundY = std::lroundf(y);

	short retX;
	short retY;
	if (roundX % 32 > 16)
		retX = ((roundX / 32) + 1);
	else
		retX = (roundX / 32);

	if (roundY % 32 > 16)
		retY = ((roundY / 32) + 1);
	else
		retY = (roundY / 32);

	return { retX, retY };
}

bool GameMap::canGo(ActorRef actor, Position dest)
{
	auto [x, y] = dest;
	if (actor->mPos == dest)
		return false;

	if (isInRange(dest) == false)
		return false;

#ifndef TEST
	//Position shortPos = SearchMapPosition(dest);
	//if (IsOccupied(shortPos.x, shortPos.y))
	//	return false;

	if (auto room = mRoom.lock())
		if (room->IsCollision(actor, dest))
			return false;
#endif // !TEST

	return true;
}


bool GameMap::isInRange(Position dest)
{
	return mMapRange[0] * 32 <= dest.x && dest.x < mMapRange[1] * 33 && mMapRange[2] * 32 <= dest.y && dest.y < mMapRange[3] * 30;
}

#pragma endregion


#pragma region 초기화
bool GameMap::Init(RoomRef room)
{
	mRoom = room;

	if(loadData() == false) 
		return false;

	return true;
}

bool GameMap::loadData()
{	

#ifdef TEST
	std::string mapFileName = gConfigManager->ProjectPath + "\\Config\\Map_Test.csv";
	std::string collisionFileName = gConfigManager->ProjectPath + "\\Config\\MapCollision_Test.csv";
#else
	std::string mapFileName = gConfigManager->ProjectPath + "\\Config\\Map_" + to_string(mId) + ".csv";
	std::string collisionFileName = gConfigManager->ProjectPath + "\\Config\\MapCollision_" + to_string(mId) + ".csv";
#endif // TEST

	ifstream mapfile(mapFileName);
	if (true == mapfile.fail())
	{
		//파일 읽기실패 TODO:로그 예외처리
		return false;
	}

	ifstream collisionFile(collisionFileName);
	if (true == collisionFile.fail())
	{
		//파일 읽기실패 TODO:로그 예외처리
		return false;
	}



	string cell;
	string line;

	for (auto i = 0; i < 4; ++i)
	{
		getline(mapfile, line);
		stringstream lineStream(line);
		getline(lineStream, cell, ',');
		mMapRange.push_back(stoi(cell));
	}

	mMap.reserve(mMapRange[3]);
	mMapCollision.reserve(mMapRange[3]);

	while (getline(mapfile, line))
	{
		auto maxX = mMapRange[1];
		Vector<short> row;
		row.reserve(maxX);
		stringstream lineStream(line);


		while (getline(lineStream, cell, ','))
		{
			row.push_back((short)stoi(cell));
		}

		mMap.push_back(row);
	}

	while (getline(collisionFile, line))
	{
		auto maxX = mMapRange[1];
		Vector<short> row;
		row.reserve(maxX);
		stringstream lineStream(line);


		while (getline(lineStream, cell, ','))
		{
			row.push_back((short)stoi(cell));
		}

		mMapCollision.push_back(row);
	}

	return true;
}

bool GameMap::SpawnMapActor()
{
	uint8_t gameScale = 2;
	Position BGPos(mMapRange[1] * 32 * gameScale / 2, (mMapRange[3] - 1) * 32 * gameScale / 2);

	auto bgActor = MakeShared<BG>();
	bgActor->SetPosition(BGPos);

	if (auto room = mRoom.lock())
	{
		bgActor->mId = 0;
		room->Spawn(bgActor);

		for (auto i = 0; i < mMap.size(); ++i)
			for (auto j = 0; j < mMap[0].size(); ++j)
			{
				auto blockType = static_cast<Protocol::BlockType>(mMap[i][j]);
				if (blockType == Protocol::BlockType::BLOCK_TYPE_NONE)
					continue;

				auto blockRef = MakeShared<Block>(blockType);
				blockRef->mName = "";
				blockRef->mType = Protocol::ActorType::ACTOR_TYPE_BLOCK;
				blockRef->mBlockType = blockType;

				blockRef->SetPosition(Position(i * 32.f, j * 32.f));

				blockRef->mId = room->actorId++;

				room->Spawn(blockRef);
			}

		return true;
	}
	else
	{
		return false;
	}
}
#pragma endregion
