#pragma once
#include "stdafx.h"

using PACKET_TYPE_SIZE = size_t;

enum class ePacketType : PACKET_TYPE_SIZE
{
	NONE,
	CS_REQ_EXIT,
	SC_RES_EXIT,
	CS_NOTIFY_HEARTBEAT,
};