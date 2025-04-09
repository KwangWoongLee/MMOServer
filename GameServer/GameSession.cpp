#include "stdafx.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "PacketHandler.h"
#include "Player.h"
#include "User.h"
#include "RoomManager.h"

 uint16_t MAX_PACKET_ID = 10;

GameSession::GameSession()
{
}
void GameSession::OnConnected()
{
	gSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(PacketHeader header, google::protobuf::io::CodedInputStream& payloadInputStream)
{
	if (header.id >= MAX_PACKET_ID || header.id < 0)
	{
		DisConnect("Error Packet ID");
		return;
	}
	auto packetSessionRef = static_pointer_cast<PacketSession>(shared_from_this());

	HandleFuncs[header.id](packetSessionRef, header, payloadInputStream);
}

void GameSession::OnSend(uint32_t transferred)
{
}

void GameSession::OnDisconnected()
{
	std::cout << "Client DisConnect" << std::endl;

	if (mUser != nullptr) {
		auto room = gRoomManager->Find(mUser->mRoomId);
		if (room != nullptr)
		{
			room->DoAsync(&Room::Leave, mUser);
		}

		mUser = nullptr;
	};

	gSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}