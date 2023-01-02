#include "stdafx.h"

#pragma region Session Virtual

void Session::Dispatch(Overlapped* iocpEvent, uint32 numOfBytes)
{
	auto type = iocpEvent->GetType();
	switch (type)
	{
	case eIOType::CONNECT:
		break;
	case eIOType::DISCONNECT:
		break;
	case eIOType::SEND:
		OnSendCompleted(numOfBytes);
		break;
	case eIOType::RECV:
		OnRecvCompleted(numOfBytes);
		break;
	
	default:
		break;
	}
}
#pragma endregion



void Session::Init(const SOCKET& socket)
{
	mSocket = socket;
}

bool Session::SetSockAddr()
{
	SocketAddress sockAddress;
	auto name = sockAddress.GetSockAddr();
	int nameSize = sizeof(sockAddress.GetSockAddr());

	if (::getpeername(mSocket, &name, &nameSize) == SOCKET_ERROR)
		return false;

	mSockAddress = sockAddress;

	return true;
}

void Session::AsyncDisconnect(const char* reason)
{
	//DisConnectOverlapped* disConnectOverlapped = new DisConnectOverlapped;
	//disConnectOverlapped->mDisConnectReason = reason;

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
	if (mConnected == false)
		return;

	// WSARecv에 등록 후 Complete 될 때까지 Session 유지를 위해, Ref Count 증가
	mRecvEvent.Init();
	mRecvEvent.SetOwner(shared_from_this()); 
	
	WSABUF wsaBuf;
	wsaBuf.buf = mRecvBuff.GetBuffer();
	wsaBuf.len = static_cast<ULONG>(mRecvBuff.GetFreeSpaceSize());

	DWORD flags = 0;
	DWORD recvBytes = 0;

	auto error = WSARecv(mSocket, &wsaBuf, 1, &recvBytes, &flags, &mRecvEvent, NULL);
	if (error == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// Pending 외에 다른 오류라면 잘못된것
			// TODO :: 에러처리

			//실패했기 때문에 Ref Count 해제
			mRecvEvent.SetOwner(nullptr);
		}
	}
}

void Session::AsyncSend()
{
	if (mConnected == false)
		return;

	mSendEvent.Init();
	mSendEvent.SetOwner(shared_from_this()); // ADD_REF

	//{
	//	//TODO :: LOCK

	//	uint32 writeSize = 0;
	//	while (mSendQueue.empty() == false)
	//	{
	//		SendBufferRef sendBuffer = mSendQueue.front();

	//		writeSize += sendBuffer->GetWriteSize();
	//		// TODO : 예외 체크

	//		mSendQueue.pop();
	//		mSendEvent.GetSendBuffers().push_back(sendBuffer);
	//	}
	//}

	//Vector<WSABUF> wsaBufs;
	//wsaBufs.reserve(mSendEvent.GetSendBuffers().size());
	//for (SendBufferRef sendBuffer : mSendEvent.GetSendBuffers())
	//{
	//	WSABUF wsaBuf;
	//	wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
	//	wsaBuf.len = static_cast<LONG>(sendBuffer->GetWriteSize());
	//	wsaBufs.push_back(wsaBuf);
	//}

	//DWORD flags = 0;
	//DWORD sendBytes;
	//DWORD error = WSASend(mSocket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), &sendBytes, flags, &mSendEvent, NULL);

	//if (error == SOCKET_ERROR)
	//{
	//	if (WSAGetLastError() != WSA_IO_PENDING)
	//	{
	//		mSendEvent.SetOwner(nullptr);
	//		mSendEvent.GetSendBuffers().clear();
	//		mSendFlag.store(false);

	//		auto err = std::format("Send Error {}", WSAGetLastError());
	//		std::cout << err << std::endl;
	//	}
	//}
}

void Session::OnAcceptCompleted()
{
	mConnected = true;

	OnConnected();

	AsyncRecv();
}

void Session::OnConnectCompleted()
{
}

void Session::OnRecvCompleted(uint32 transferred)
{
	// WSARecv가 종료 되었으므로 Ref Count를 풀어줌
	mRecvEvent.SetOwner(nullptr);

	// 이 아래 상황에는 Recv하지 않으므로 여기 다시 안오기 때문에
	// No Lock

	// 클라이언트가 강제종료 시, 0바이트 전송
	if (transferred == 0)
	{
		AsyncDisconnect("Transfered Byte Zero By Client");
		return;
	}


	// WSARecv 등록 시, 최대 Freespace만큼 받을 수 있도록 등록했는데 초과됨, 더 받음
	uint32 freeSize = mRecvBuff.GetFreeSpaceSize();
	if (transferred > freeSize)
	{
		AsyncDisconnect("Recv Buffer Over Write");
		return;
	}


	uint32 processLen = OnRecv();
	// 패킷 분석을 통해, 해당 패킷을 다 읽고 읽은 양을 뱉어냈는데, 그 양이 저장된 데이터보다 많음, 더 읽음
	if (processLen < 0 || mRecvBuff.GetContiguiousBytes() < processLen)
	{
		this->AsyncDisconnect("Recv Buffer Over Read");
		return;
	}

	// 여기까지 괜찮다면, 처리한 데이터만큼 기록 포지션 증가
	mRecvBuff.Commit(processLen);


	// 한번에 비동기 Recv 한번만, 끝났으니 다시 걸어주기
	AsyncRecv();
}

void Session::OnSendCompleted(uint32 transferred)
{
	//{
	//	std::unique_lock<std::mutex>(mSendQueueLock);

	//	OnSend(transferred);

	//	if (!mSendQueue.empty())
	//		this->AsyncSend();
	//}
}

void Session::Send()
{
	//{
	//	std::unique_lock<std::mutex>(mSendQueueLock);

	//	mSendQueue.push(packet);

	//	if (!mSendQueue.empty())
	//		this->AsyncSend();
	//}
}

uint32 PacketSession::OnRecv()
{
	//패킷분석
	//완료 안되면 다시

	uint32 processLen = 0;

	auto recvBuff = GetRecvBuffer();
	//패킷 분석을 위해 헤더크기 이상이 있는지 확인

	// enable == true 이면, id, size 값이 들어있고, 패킷 조립이 가능한 상태
	auto [enable, id, contentSize] = PacketAnalyzer::Analyze(recvBuff);
	
	// 조립해서 패킷처리는 자식클래스에 위임
	OnRecvPacket(const_cast<const char*>(recvBuff.GetBufferStart()), id, contentSize);
	recvBuff.Remove(contentSize);

	//여기까지 오면 한 패킷 전체 처리 다한 것, 패킷 크기 리턴해준다
	processLen += sizeof(PacketHeader) + contentSize;

	return processLen;
}

void PacketSession::OnSend(uint32 numOfBytes)
{
}

void PacketSession::OnConnected()
{
	
}

void PacketSession::OnDisconnected(const char* reason)
{
}
