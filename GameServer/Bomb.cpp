#include "stdafx.h"
#include "Bomb.h"

void Bomb::Update()
{
}

bool Bomb::IsInRange(ActorRef actor)
{
	auto [myX, myY] = mPos;
	auto [destX, destY] = actor->mPos;

	float fDistanceX = fabsf(myX - destX);
	float fDistanceY = fabsf(myY - destY);

	if (fDistanceY == 0 && fDistanceX <= mPower * 32.f)
	{
		return true;
	}

	if (fDistanceX == 0 && fDistanceY <= mPower * 32.f)
	{
		return true;
	}

	if (actor->mType == Protocol::ACTOR_TYPE_PLAYER)
	{
		if (fDistanceX <= mPower * 32.f && fDistanceY <= mPower * 32.f)
		{
			return true;
		}
	}

	return false;
}
