#include "stdafx.h"
#include "Player.h"

Player::~Player()
{
	if (auto room = GetRoom())
	{
		//room->DoAsync(&Room::CheckQuit, );
	}
}

void Player::Update()
{

};


