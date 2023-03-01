#include "stdafx.h"
#include "GameManager.h"
#include "RoomManager.h"

std::shared_ptr<GameManager> gGameManager = MakeShared<GameManager>();

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::Update()
{
	DoTimer(250, &GameManager::Update);

	gRoomManager->Update();
}
