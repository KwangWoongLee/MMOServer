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

	// WSARecv�� ��� �� Complete �� ������ Session ������ ����, Ref Count ����
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
			// Pending �ܿ� �ٸ� ������� �߸��Ȱ�
			// TODO :: ����ó��

			//�����߱� ������ Ref Count ����
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
	//		// TODO : ���� üũ

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
	// WSARecv�� ���� �Ǿ����Ƿ� Ref Count�� Ǯ����
	mRecvEvent.SetOwner(nullptr);

	// �� �Ʒ� ��Ȳ���� Recv���� �����Ƿ� ���� �ٽ� �ȿ��� ������
	// No Lock

	// Ŭ���̾�Ʈ�� �������� ��, 0����Ʈ ����
	if (transferred == 0)
	{
		AsyncDisconnect("Transfered Byte Zero By Client");
		return;
	}


	// WSARecv ��� ��, �ִ� Freespace��ŭ ���� �� �ֵ��� ����ߴµ� �ʰ���, �� ����
	uint32 freeSize = mRecvBuff.GetFreeSpaceSize();
	if (transferred > freeSize)
	{
		AsyncDisconnect("Recv Buffer Over Write");
		return;
	}


	uint32 processLen = OnRecv();
	// ��Ŷ �м��� ����, �ش� ��Ŷ�� �� �а� ���� ���� ���´µ�, �� ���� ����� �����ͺ��� ����, �� ����
	if (processLen < 0 || mRecvBuff.GetContiguiousBytes() < processLen)
	{
		this->AsyncDisconnect("Recv Buffer Over Read");
		return;
	}

	// ������� �����ٸ�, ó���� �����͸�ŭ ��� ������ ����
	mRecvBuff.Commit(processLen);


	// �ѹ��� �񵿱� Recv �ѹ���, �������� �ٽ� �ɾ��ֱ�
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
	//��Ŷ�м�
	//�Ϸ� �ȵǸ� �ٽ�

	uint32 processLen = 0;

	auto recvBuff = GetRecvBuffer();
	//��Ŷ �м��� ���� ���ũ�� �̻��� �ִ��� Ȯ��

	// enable == true �̸�, id, size ���� ����ְ�, ��Ŷ ������ ������ ����
	auto [enable, id, contentSize] = PacketAnalyzer::Analyze(recvBuff);
	
	// �����ؼ� ��Ŷó���� �ڽ�Ŭ������ ����
	OnRecvPacket(const_cast<const char*>(recvBuff.GetBufferStart()), id, contentSize);
	recvBuff.Remove(contentSize);

	//������� ���� �� ��Ŷ ��ü ó�� ���� ��, ��Ŷ ũ�� �������ش�
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
