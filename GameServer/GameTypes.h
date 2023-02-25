#pragma once
#include "stdafx.h"
#include "Proto/struct.pb.h"

enum class BlockType
{
	BLOCK_TYPE_NONE = 0,
	BLOCK_TYPE_BUSH1 = 3,
	BLOCK_TYPE_BUSH2 = 4,
	BLOCK_TYPE_BUSH3 = 8,
	BLOCK_TYPE_BUSH4 = 9,
	BLOCK_TYPE_BUSH5 = 16,
	BLOCK_TYPE_BUSH6 = 17,
	BLOCK_TYPE_BUSH7 = 18,

};

using GameSessionRef = shared_ptr<class GameSession>;
using ActorRef = shared_ptr<class Actor>;
using BombRef = shared_ptr<class Bomb>;
using PlayerRef = shared_ptr<class Player>;
using UserRef = shared_ptr<class User>;
using RoomRef = shared_ptr<class Room>;