#include "stdafx.h"
#include "GameMap.h"
#include "Actor.h"
#include "Room.h"
#include "Block.h"


#include <fstream>
#include <sstream>

//#include "Block.h"

GameMap::GameMap(uint8 id)
	: mId(id)
{
}

#pragma region �̵�
bool GameMap::ApplyMove(ActorRef actor, Position dest)
{
	if (actor == nullptr)
		return false;

	if (canGo(actor, dest) == false)
		return false;

	actor->mPos = dest;
	return true;
}

bool GameMap::canGo(ActorRef actor, Position dest)
{
	auto [x, y] = dest;
	if (actor->mPos == dest)
		return false;

	if (isInRange(dest) == false)
		return false;

	if (auto room = mRoom.lock())
		if (room->IsCollision(actor, dest))
			return false;

	return true;
}


bool GameMap::isInRange(Position dest)
{
	return mMapRange[0] * 32 <= dest.x && dest.x < mMapRange[1] * 33 && mMapRange[2] * 32 <= dest.y && dest.y < mMapRange[3] * 30;
}

#pragma endregion


#pragma region �ʱ�ȭ
bool GameMap::Init(RoomRef room)
{
	mRoom = room;

	if(loadData() == false) 
		return false;
	//if(spawnMapActor() == false) 
	//	return false;

	return true;
}

bool GameMap::loadData(std::string_view mapFileName, std::string_view collisionFileName)
{
	ifstream mapfile(mapFileName.data());
	if (true == mapfile.fail())
	{
		//���� �б���� TODO:�α� ����ó��
		return false;
	}

	ifstream collisionFile(collisionFileName.data());
	if (true == collisionFile.fail())
	{
		//���� �б���� TODO:�α� ����ó��
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
		vector<short> row;
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
		vector<short> row;
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

bool GameMap::spawnMapActor()
{
	uint8 gameScale = 2;
	Position BGPos(mMapRange[1] * 32 * gameScale / 2, (mMapRange[3] - 1) * 32 * gameScale / 2);

	auto bgActor = make_shared<BG>();
	bgActor->SetPosition(BGPos);

	if (auto room = mRoom.lock())
	{
		bgActor->mId = 0;

		for (auto i = 0; i < mMap.size(); ++i)
			for (auto j = 0; j < mMap[0].size(); ++j)
			{
				auto blockType = static_cast<Protocol::BlockType>(mMap[i][j]);
				if (blockType == Protocol::BlockType::BLOCK_TYPE_NONE)
					continue;

				auto blockRef = std::make_shared<Block>(blockType);
				blockRef->mName = "";
				blockRef->mType = Protocol::ActorType::ACTOR_TYPE_BLOCK;
				blockRef->mBlockType = blockType;

				blockRef->SetPosition(Position(i * 32.f, j * 32.f));

				blockRef->mId = room->actorId++;
			}

		return true;
	}
	else
	{
		return false;
	}
}
#pragma endregion