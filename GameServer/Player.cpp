#include "stdafx.h"
#include "Player.h"

Player::~Player()
{
}

void Player::Update()
{

};

//
//Position Player::SearchBombPosition()
//{
//	auto [x, y] = mPos;
//	auto roundX = std::lroundf(x);
//	auto roundY = std::lroundf(y);
//
//	int retX;
//	int retY;
//	if (roundX % 32 > 16)
//		retX = ((roundX / 32) + 1) * 32;
//	else
//		retX = (roundX / 32) * 32;
//
//	if (roundY % 32 > 16)
//		retY = ((roundY / 32) + 1) * 32;
//	else
//		retY = (roundY / 32) * 32;
//
//	return { retX, retY };
//}