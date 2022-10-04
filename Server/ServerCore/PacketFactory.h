#pragma once
#include "stdafx.h"

class PacketFactory : public Singleton<PacketFactory>
{
public:
	std::shared_ptr<Packet> GetPacket(ePacketType type)
	{
		switch (type)
		{
		case ePacketType::SC_RES_EXIT:
			return std::make_shared<PK_SC_RES_EXIT>();
			break;

		case ePacketType::CS_REQ_EXIT:
			return std::make_shared<PK_CS_REQ_EXIT>();
			break;

		case ePacketType::CS_NOTIFY_HEARTBEAT:
			return std::make_shared<PK_CS_NOTIFY_HEARTBEAT>();
			break;


		default:
			return nullptr;
			break;
		}
	}
};