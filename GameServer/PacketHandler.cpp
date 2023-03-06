#include "stdafx.h"
#include "PacketHandler.h"
#include "Proto/protocol.pb.h"

#include "GameSession.h"
#include "RoomManager.h"
#include "Actor.h"
#include "Player.h"
#include "Room.h"
#include "User.h"
#include <random>

std::vector<function<void(PacketSessionRef, PacketHeader, google::protobuf::io::CodedInputStream&)>> HandleFuncs(UINT16_MAX);
uint64 aidx = 1;

void PacketHandler::Init()
{
	HandleFuncs[0] = &PacketHandler::C_PONG;
	HandleFuncs[1] = &PacketHandler::C_ENTER_GAME;
	HandleFuncs[2] = &PacketHandler::C_ACTION;
}

void PacketHandler::C_PONG(PacketSessionRef session, PacketHeader header, google::protobuf::io::CodedInputStream& inputStream)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	gameSession->GetUser()->mPing = GetTickCount64();
}

void PacketHandler::C_ENTER_GAME(PacketSessionRef session, PacketHeader header, google::protobuf::io::CodedInputStream& inputStream)
{
	Protocol::C_ENTER_GAME pkt;

	if (false == pkt.ParseFromCodedStream(&inputStream))
	{
		session->DisConnect("Packet Parse Failed");
		return;
	}

	uint64 aidx = pkt.aidx();
	string name = pkt.nickname();
	uint32 roomId = pkt.roomid();

	auto roomRef = gRoomManager->Find(roomId);
	if (roomRef == nullptr)
	{
		session->DisConnect("Room Is Empty");
		return;
	}

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	if (aidx == 9999)
	{
		roomRef->DoTimer(500, &Room::Test, gameSession);
		return;
	}




	PlayerRef newPlayer = MakeShared<Player>(pkt.playertype());
	newPlayer->mView.SetOwner(newPlayer);

	Position pos = {};
	//{
	//  //·£´ý Æ÷Áö¼Ç
	//	std::random_device rd;
	//	std::mt19937 gen(rd());
	//	std::uniform_int_distribution<int> dis(1, 11);

	//	pos = { dis(gen) * 32.f,dis(gen) * 32.f };
	//}

	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(0, 3);

		std::vector<Position> startPos = { {1 * 32.f,11 * 32.f}, {1 * 32.f,1 * 32.f}, {11 * 32.f,1 * 32.f}, {11 * 32.f,12 * 32.f} };

		pos = startPos[dis(gen)];
	}
	
	newPlayer->SetPosition(pos, false);
	newPlayer->SetRoom(roomRef);
	

	UserRef newUser = MakeShared<User>(gameSession, aidx, name, roomId, newPlayer);
	gameSession->SetUser(newUser);

	roomRef->DoAsync(&Room::Enter, newUser);
}

void PacketHandler::C_ACTION(PacketSessionRef session, PacketHeader header, google::protobuf::io::CodedInputStream& inputStream)
{
	Protocol::C_ACTION pkt;

	if (false == pkt.ParseFromCodedStream(&inputStream))
	{
		session->DisConnect("Packet Parse Failed");
		return;
	}

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession->GetUser() == nullptr)
		return;

	if (gameSession->GetUser()->mPlayer == nullptr)
		return;

	if (auto room = gameSession->GetUser()->mPlayer->mRoom.lock())
	{
		room->DoAsync(&Room::ApplyAction, gameSession ,gameSession->GetUser()->mPlayer, pkt);
	}

	return;
}
