#pragma once
#include "CircularBuffer.h"

using RecvBuffer = CircularBuffer;

static class PacketAnalyzer
{
public:
	static std::tuple<bool, uint16, uint16> Analyze(RecvBuffer& recvBuffer)
	{
		// 헤더를 읽을 수조차 없는 크기만큼 Recv된 경우
		uint32 enableMaxReadBytes = recvBuffer.GetContiguiousBytes();
		if (enableMaxReadBytes < sizeof(PacketHeader))
			return { false, {}, {} };

		PacketHeader header;
		char* buffer = recvBuffer.GetBufferStart();
		memcpy(reinterpret_cast<BYTE*>(&header), buffer, sizeof(PacketHeader));

		// 헤더는 읽었지만, 패킷 전체를 읽을 수 없는 경우
		if (enableMaxReadBytes >= header.size)
			return { false, {}, {} };

		// 여기까지 왔으면 앞에 sizeof(PacketHeader) 만큼은 지우기
		recvBuffer.Remove(sizeof(PacketHeader));

		auto contentSize = header.size - sizeof(PacketHeader);
		auto packetId = header.id;

		
		return { true, packetId, contentSize };
	}
};
