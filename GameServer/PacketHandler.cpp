#include "stdafx.h"
#include "PacketHandler.h"
#include "Proto/protocol.pb.h"

#include "GameSession.h"
#include "RoomManager.h"
#include "Actor.h"
#include "Player.h"
#include "Room.h"
#include "User.h"

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

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef newPlayer = std::make_shared<Player>(pkt.playertype());
	newPlayer->SetPosition({ 1.f * 32,11.f * 32 }, false);

	UserRef newUser = std::make_shared<User>(gameSession, aidx, name, roomId, newPlayer);
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
