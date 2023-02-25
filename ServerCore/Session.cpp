#include "stdafx.h"
#include "ClientEngine.h"
#include "GlobalQueue.h"

#pragma region Session Virtual

void Session::Dispatch(Overlapped* iocpEvent, uint32 numOfBytes)
{
	auto type = iocpEvent->ioType;
	switch (type)
	{
	case eIOType::ACCEPT:
		OnAcceptCompleted();
		break;
	case eIOType::CONNECT:
		OnConnectCompleted();
		break;
	case eIOType::DISCONNECT:
		OnDisConnectCompleted();
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
Session::Session()
{
	mSocket = SocketUtil::GetInstance().CreateSocket();
}


bool Session::SetSockAddr()
{
	SocketAddress sockAddress;
	auto name = reinterpret_cast<SOCKADDR*>(&sockAddress.GetSockAddr());
	int nameSize = sizeof(SOCKADDR);

	if (::getpeername(mSocket, name, &nameSize) == SOCKET_ERROR)
		return false;

	mSockAddress = sockAddress;

	return true;
}

bool Session::asyncConnect()
{
	if (mConnected.load() == true)
		return false;

	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(0);

	::bind(mSocket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));


	if (SocketUtil::GetInstance().SetReuseAddress(mSocket, true) == false)
		return false;

	auto clientEngine = static_pointer_cast<ClientEngine>(GetEngine());
	auto serverSockAddress = clientEngine->GetServerSockAddr();

	mConnectEvent.Init();
	mConnectEvent.owner = shared_from_this();

	DWORD numOfBytes = 0;
	if (FALSE == FnConnectEx(mSocket, reinterpret_cast<const SOCKADDR*>(&serverSockAddress.GetSockAddr()), sizeof(serverSockAddress.GetSockAddr()), nullptr, 0, &numOfBytes, &mConnectEvent))
	{
		auto error = WSAGetLastError();

		if (error != WSA_IO_PENDING)
		{
			mConnectEvent.owner = nullptr;
			return false;
		}
	}

	return true;
}

void Session::asyncDisconnect()
{
	mDisconnectEvent.Init();
	mDisconnectEvent.owner = shared_from_this();


	if (FALSE == FnDisconnectEx(mSocket, &mDisconnectEvent, TF_REUSE_SOCKET, 0))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			mDisconnectEvent.owner = nullptr;
		}
	}
}

void Session::AsyncRecv()
{
	if (mConnected == false)
		return;

	// WSARecv�� ��� �� Complete �� ������ Session ������ ����, Ref Count ����
	mRecvEvent.Init();
	mRecvEvent.owner = shared_from_this(); 
	
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
			auto error = WSAGetLastError();
			// Pending �ܿ� �ٸ� ������� �߸��Ȱ�
			// TODO :: ����ó��

			//�����߱� ������ Ref Count ����
			mRecvEvent.owner = nullptr;
		}
	}
}

bool Session::AsyncSend()
{
	if (mConnected == false)
		return false;
	
	{
		WRITE_LOCK;

		/// ���� �����Ͱ� ���� ���
		if (0 == mSendBuffer.GetContiguiousBytes())
		{
			/// ���� �����͵� ���� ���
			if (0 == mSendPendingCount)
				return true;

			return false;			
		}

		/// ������ send�� �Ϸ� �ȵ� ���
		if (mSendPendingCount > 0)
		{
			return false;
		}


		mSendEvent.Init();
		mSendEvent.owner = shared_from_this();

		DWORD sendbytes = 0;
		DWORD flags = 0;
		WSABUF wsaBuf;
		wsaBuf.len = (ULONG)mSendBuffer.GetContiguiousBytes();
		wsaBuf.buf = mSendBuffer.GetBufferStart();

		if (SOCKET_ERROR == WSASend(mSocket, &wsaBuf, 1, &sendbytes, flags, &mSendEvent, NULL))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				mSendEvent.owner = nullptr;

				return true;
			}
		}

		mSendPendingCount++;

		return mSendPendingCount == 1;
	}
}

void Session::OnAcceptCompleted()
{
	mConnectEvent.owner = nullptr;
	mConnected = true;

	OnConnected();

	AsyncRecv();
}

void Session::OnConnectCompleted()
{
	mConnectEvent.owner = nullptr;
	mConnected = true;

	OnConnected();

	AsyncRecv();
}

void Session::OnDisConnectCompleted()
{
	mDisconnectEvent.owner = nullptr;
	mConnected = false;

	OnDisconnected();

	GetEngine()->DisConnectSession(static_pointer_cast<Session>(shared_from_this()));
}

void Session::OnRecvCompleted(uint32 transferred)
{
	// WSARecv�� ���� �Ǿ����Ƿ� Ref Count�� Ǯ����
	mRecvEvent.owner = nullptr;

	// Ŭ���̾�Ʈ�� �������� ��, 0����Ʈ ����
	if (transferred == 0)
	{
		DisConnect("Transfered Byte Zero By Client");
		return;
	}

	// WSARecv ��� ��, �ִ� Freespace��ŭ ���� �� �ֵ��� ����ߴµ� �ʰ���, �� ����
	uint32 freeSize = mRecvBuff.GetFreeSpaceSize();
	if (transferred > freeSize)
	{
		DisConnect("Recv Buffer Over Write");
		return;
	}

	mRecvBuff.Commit(transferred);

	OnRecv();

	// �ѹ��� �񵿱� Recv �ѹ���, �������� �ٽ� �ɾ��ֱ�
	AsyncRecv();
}

void Session::OnSendCompleted(uint32 transferred)
{	
	mSendEvent.owner = nullptr;

	if (transferred == 0)
	{
		DisConnect("Send Zero Byte");
		return;
	}

	// �ڽ� session Ŭ�������� �� �۾�
	OnSend(transferred);

	{
		WRITE_LOCK;

		mSendBuffer.Remove(transferred);

		mSendPendingCount--;
	}
}

bool Session::Connect()
{
	return asyncConnect();
}

void Session::DisConnect(const char* reason)
{
	// �̹� Disconnect ���¸� �׳� ����
	if (mConnected.exchange(false) == false)
	{
		return;
	}

	//TODO : reason���� ��� �����

	asyncDisconnect();
}

bool Session::Send(const char* buffer, uint32 contentSize)
{
	if (!mConnected.load())
		return false;

	{
		WRITE_LOCK;

		if (mSendBuffer.GetFreeSpaceSize() < contentSize)
			return false;


		auto sessionRef = static_pointer_cast<Session>(shared_from_this());
		TLS_SendSessionQueue.push(sessionRef);

		char* destData = mSendBuffer.GetBuffer();
		memcpy(destData, buffer, contentSize);

		mSendBuffer.Commit(contentSize);
	}

	return true;
}

bool PacketSession::Send(uint16 packetId, google::protobuf::MessageLite& packet)
{
	if (mConnected.load() == false)
		return false;
	{
		WRITE_LOCK;


		auto contentSize = packet.ByteSizeLong();

		PacketHeader header;
		header.size = contentSize;
		header.id = packetId;
		header.tickCount = GetTickCount64();

		auto packetSize = sizeof(PacketHeader) + contentSize;

		if (mSendBuffer.GetFreeSpaceSize() < packetSize)
			return false;

		google::protobuf::io::ArrayOutputStream arrayOutputStream(mSendBuffer.GetBuffer(), packetSize);
		google::protobuf::io::CodedOutputStream codedOutputStream(&arrayOutputStream);


		codedOutputStream.WriteRaw(&header, sizeof(PacketHeader));
		packet.SerializeToCodedStream(&codedOutputStream);


		auto sessionRef = static_pointer_cast<Session>(shared_from_this());
		TLS_SendSessionQueue.push(sessionRef);

		mSendBuffer.Commit(packetSize);
	}
	
	return true;
}

bool PacketSession::OnRecv()
{
	//��Ŷ�м�
	//�Ϸ� �ȵǸ� �ٽ�
	google::protobuf::io::ArrayInputStream arrayInputStream(mRecvBuff.GetBufferStart(), mRecvBuff.GetContiguiousBytes());
	google::protobuf::io::CodedInputStream codedInputStream(&arrayInputStream);

	PacketHeader packetheader;

	while (codedInputStream.ReadRaw(&packetheader, sizeof(PacketHeader)))
	{
		const void* payloadPos = nullptr;
		int payloadSize = 0;

		codedInputStream.GetDirectBufferPointer(&payloadPos, &payloadSize);

		if (payloadSize < packetheader.size) ///< ��Ŷ ��ü ������ üũ
			return false;

		/// payload �б�
		google::protobuf::io::ArrayInputStream payloadArrayStream(payloadPos, packetheader.size);
		google::protobuf::io::CodedInputStream payloadInputStream(&payloadArrayStream);


		OnRecvPacket(packetheader, payloadInputStream);

		/// ���� ��ŭ ���� �� ���ۿ��� ����
		codedInputStream.Skip(packetheader.size); ///< readraw���� ��� ũ�⸸ŭ �̸� �����߱⶧��
		mRecvBuff.Remove(sizeof(PacketHeader) + packetheader.size);
	}

	return true;
}
