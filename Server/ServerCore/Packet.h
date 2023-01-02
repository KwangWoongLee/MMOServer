#pragma once
#include "stdafx.h"

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

//
//class Packet
//{
//public:
//	Packet(ePacketType type)
//		: mType(type) {};
//
//	ePacketType				GetType() { return mType; }
//	PACKET_TYPE_SIZE		GetTypeToInt() { return static_cast<PACKET_TYPE_SIZE>(mType); }
//
//	virtual void Encode(OutputStream& stream) {};
//	virtual void Decode(InputStream& stream) {};
//
//
//private:
//	ePacketType mType;
//
//};
//
//
//class PK_CS_REQ_EXIT : public Packet
//{
//public:
//	PK_CS_REQ_EXIT()
//		:Packet(ePacketType::CS_REQ_EXIT) {};
//
//	void Encode(OutputStream& stream) override;
//
//
//
//};
//
//
//class PK_SC_RES_EXIT : public Packet
//{
//public:
//	PK_SC_RES_EXIT()
//		:Packet(ePacketType::SC_RES_EXIT) {};
//
//	void Encode(OutputStream& stream) override;
//
//
//
//};
//
//
//class PK_CS_NOTIFY_HEARTBEAT : public Packet
//{
//public:
//	PK_CS_NOTIFY_HEARTBEAT()
//		:Packet(ePacketType::CS_NOTIFY_HEARTBEAT) {};
//
//	void Encode(OutputStream& stream) override;
//
//};
