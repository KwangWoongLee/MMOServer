#include "stdafx.h"
#include "User.h"

#include "GameSession.h"
#include "Player.h"

User::User(GameSessionRef session, uint64 aidx, string name, uint32 roomId, PlayerRef player)
	: mSession(session),
	mAidx(aidx),
	mName(name),
	mRoomId(roomId),
	mPlayer(player)
{
}

User::~User()
{
	mPlayer = nullptr;
}
