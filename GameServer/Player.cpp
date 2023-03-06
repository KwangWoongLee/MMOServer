#include "stdafx.h"
#include "Player.h"

Player::~Player()
{
	cout << "~Player()" << endl;
	if (auto room = GetRoom(); room != nullptr)
	{
		if(room->mPlayers.find(mId) != room->mPlayers.end())
			room->mPlayers.erase(mId);
	}
		
}

void Player::Update()
{

};


