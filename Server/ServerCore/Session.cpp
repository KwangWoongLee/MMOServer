#include "stdafx.h"

void Session::Init(const SOCKET& socket)
{
	mSocket = socket;
}

void Session::AsyncDisconnect(eDisConnectReason reason)
{
	DisConnectOverlapped* disConnectOverlapped = new DisConnectOverlapped;
	disConnectOverlapped->mDisConnectReason = reason;

	//if (FALSE == FnDisConnectEx(mSocket, (LPWSAOVERLAPPED)disConnectOverlapped, TF_REUSE_SOCKET, 0))
	//{
	//	if (WSAGetLastError() != WSA_IO_PENDING)
	//	{
	//		delete disConnectOverlapped;

	//		auto err = std::format("DisConnect Error {}", WSAGetLastError());
	//		std::cout << err << std::endl;
	//	}
	//}
}

void Session::AsyncRecv()
{
	RecvOverlapped* recvOverlapped = new RecvOverlapped;
	
	auto wsaBuf = recvOverlapped->mWSABuf;

	this->mRecvBuff.Clean();
	wsaBuf.buf = mRecvBuff.GetWriteLocation();
	wsaBuf.len = static_cast<ULONG>(mRecvBuff.GetFreeSize());

	DWORD flags = 0;
	DWORD recvBytes = 0;

	auto error = WSARecv(mSocket, &wsaBuf, 1, &recvBytes, &flags, (LPOVERLAPPED)recvOverlapped, NULL);
	if (error == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// Pending 외에 다른 오류라면 잘못된것
			delete recvOverlapped;

			auto err = std::format("Recv Error {}", WSAGetLastError());
			std::cout << err << std::endl;
		}
	}
}

void Session::AsyncSend()
{
	OutputStream stream;

	while (!mSendQueue.empty())
	{
		auto packet = mSendQueue.front();
		mSendQueue.pop();
		
		packet->Encode(stream);
	}

	//패킷 총길이
	PACKET_SIZE packetTotalLength[1] = { sizeof(PACKET_SIZE) + stream.GetLength() };


	size_t	offset = sizeof(PACKET_SIZE);

	//패킷 총길이 입력
	memcpy((void*)mSendBuffer, (void*)packetTotalLength, sizeof(PACKET_SIZE));

	// 스트림 데이터 입력
	memcpy((void*)(mSendBuffer + offset), (void*)stream.GetBuffer(), stream.GetLength());


	SendOverlapped* sendOverlapped = new SendOverlapped;
	auto wsaBuf = sendOverlapped->mWSABuf;

	wsaBuf.len = (ULONG)packetTotalLength[0];
	wsaBuf.buf = mSendBuffer;


	DWORD flags = 0;
	DWORD sendBytes;
	DWORD error = WSASend(mSocket, &wsaBuf, 1, &sendBytes, flags, (LPOVERLAPPED)sendOverlapped, NULL);

	if (error == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// Pending 외에 다른 오류라면 잘못된것
			delete sendOverlapped;

			auto err = std::format("Send Error {}", WSAGetLastError());
			std::cout << err << std::endl;
		}
	}
}

void Session::OnAcceptCompleted()
{
	//SessionManager::GetInstance().Add(std::move(session));

	IOCP::GetInstance().RegistCompletionPort(mSocket, reinterpret_cast<ULONG_PTR>(this));

	this->OnConnected();

	this->AsyncRecv();

	auto packet = new Packet(ePacketType::CS_NOTIFY_HEARTBEAT);
	this->Send(packet);
}

void Session::OnRecvCompleted(size_t transferred)
{
	if (mRecvBuff.OnWrite(transferred) == false)
	{
		this->AsyncDisconnect(eDisConnectReason::IO_COMPLETE_ERROR);
		return;
	}

	int processLen = OnRecv();
	if (processLen < 0 || mRecvBuff.GetDataSize() < processLen)
	{
		this->AsyncDisconnect(eDisConnectReason::IO_COMPLETE_ERROR);
		return;
	}

	if (mRecvBuff.OnRead(processLen) == false)
	{
		this->AsyncDisconnect(eDisConnectReason::IO_COMPLETE_ERROR);
		return;
	}

	this->AsyncRecv();
}

void Session::OnSendCompleted(size_t transferred)
{
	{
		std::unique_lock<std::mutex>(mSendQueueLock);

		OnSend(transferred);

		if (!mSendQueue.empty())
			this->AsyncSend();
	}
}

void Session::Send(Packet* packet)
{
	{
		std::unique_lock<std::mutex>(mSendQueueLock);

		mSendQueue.push(packet);

		if (!mSendQueue.empty())
			this->AsyncSend();
	}
}

int PacketSession::OnRecv()
{
	//패킷분석
	//완료 안되면 다시

	int processLen = 0;

	auto recvBuff = GetRecvBuffer();
	//패킷 분석을 위해 헤더크기 이상이 있는지 확인
	if (recvBuff.GetDataSize() < sizeof(PACKET_SIZE))
		return processLen;

	//패킷 조립
	size_t dataSize = 0;
	int offset = 0;
	memcpy(&dataSize, recvBuff.GetRecvLocation(), sizeof(PACKET_SIZE));

	if (recvBuff.GetDataSize() - sizeof(PACKET_SIZE) < dataSize)
		return processLen;

	// 패킷 하나 완성 가능 상태
	OnRecvPacket(recvBuff.GetRecvLocation());

	processLen += sizeof(PACKET_SIZE) + dataSize;

	return processLen;
}

void PacketSession::OnSend(size_t numOfBytes)
{
}

void PacketSession::OnConnected()
{
	
}

void PacketSession::OnDisconnected(eDisConnectReason reason)
{
}

void ClientSession::OnRecvPacket(const char* buffer)
{
	size_t offset = 0;
	PACKET_SIZE packetSize[1] { 0 };

	memcpy((void*)packetSize, (void*)buffer, sizeof(PACKET_SIZE));

	offset += sizeof(PACKET_SIZE);

	std::shared_ptr<Packet> packet = PacketAnalyzer::GetInstance().analyze(buffer + offset, packetSize[0] - offset);

}
