#pragma once
#include "CircularBuffer.h"

using RecvBuffer = CircularBuffer;

static class PacketAnalyzer
{
public:
	static std::tuple<bool, uint16, uint16> Analyze(RecvBuffer& recvBuffer)
	{
		// ����� ���� ������ ���� ũ�⸸ŭ Recv�� ���
		uint32 enableMaxReadBytes = recvBuffer.GetContiguiousBytes();
		if (enableMaxReadBytes < sizeof(PacketHeader))
			return { false, {}, {} };

		PacketHeader header;
		char* buffer = recvBuffer.GetBufferStart();
		memcpy(reinterpret_cast<BYTE*>(&header), buffer, sizeof(PacketHeader));

		// ����� �о�����, ��Ŷ ��ü�� ���� �� ���� ���
		if (enableMaxReadBytes >= header.size)
			return { false, {}, {} };

		// ������� ������ �տ� sizeof(PacketHeader) ��ŭ�� �����
		recvBuffer.Remove(sizeof(PacketHeader));

		auto contentSize = header.size - sizeof(PacketHeader);
		auto packetId = header.id;

		
		return { true, packetId, contentSize };
	}
};
