#pragma once
#include "stdafx.h"

class PacketAnalyzer : public Singleton<PacketAnalyzer>
{
public:
	std::shared_ptr<Packet> analyze(const char* buffer, size_t size);

};
