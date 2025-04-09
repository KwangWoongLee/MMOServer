#include "stdafx.h"
#include "ClientEngine.h"
#include "GlobalQueue.h"

#pragma region Session Virtual

void Session::Dispatch(Overlapped* iocpEvent, uint32_t numOfBytes)
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

	mSendBuffer = ObjectPool<CircularBuffer>::MakeShared(65535);
	mRecvBuff = ObjectPool<CircularBuffer>::MakeShared(65535);
}

Session::~Session()
{
	mSendBuffer = nullptr;
	mRecvBuff = nullptr;
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

	// WSARecv에 등록 후 Complete 될 때까지 Session 유지를 위해, Ref Count 증가
	mRecvEvent.Init();
	mRecvEvent.owner = shared_from_this(); 
	
	WSABUF wsaBuf;
	wsaBuf.buf = mRecvBuff->GetBuffer();
	wsaBuf.len = static_cast<ULONG>(mRecvBuff->GetFreeSpaceSize());

	DWORD flags = 0;
	DWORD recvBytes = 0;

	auto error = WSARecv(mSocket, &wsaBuf, 1, &recvBytes, &flags, &mRecvEvent, NULL);
	if (error == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			auto error = WSAGetLastError();
			// Pending 외에 다른 오류라면 잘못된것
			// TODO :: 에러처리

			//실패했기 때문에 Ref Count 해제
			mRecvEvent.owner = nullptr;
		}
	}
}

void Session::AsyncSend()
{
	if (mConnected == false)
		return;

	{
		WRITE_LOCK;

		/// 보낼 데이터가 없는 경우
		if (0 == mSendBuffer->GetContiguiousBytes())
		{
			mSendEvent.owner = nullptr;
			mSendRegistered.store(false);
			return;			
		}

		mSendEvent.Init();
		mSendEvent.owner = shared_from_this();

		DWORD sendbytes = 0;
		DWORD flags = 0;
		WSABUF wsaBuf;
		wsaBuf.len = (ULONG)mSendBuffer->GetContiguiousBytes();
		wsaBuf.buf = mSendBuffer->GetBufferStart();

		if (SOCKET_ERROR == WSASend(mSocket, &wsaBuf, 1, &sendbytes, flags, &mSendEvent, NULL))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				mSendEvent.owner = nullptr;
				mSendRegistered.store(false);
			}
		}
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

void Session::OnRecvCompleted(uint32_t transferred)
{
	// WSARecv가 종료 되었으므로 Ref Count를 풀어줌
	mRecvEvent.owner = nullptr;

	// 클라이언트가 강제종료 시, 0바이트 전송
	if (transferred == 0)
	{
		DisConnect("Transfered Byte Zero By Client");
		return;
	}

	// WSARecv 등록 시, 최대 Freespace만큼 받을 수 있도록 등록했는데 초과됨, 더 받음
	uint32_t freeSize = mRecvBuff->GetFreeSpaceSize();
	if (transferred > freeSize)
	{
		DisConnect("Recv Buffer Over Write");
		return;
	}

	mRecvBuff->Commit(transferred);

	OnRecv();

	// 한번에 비동기 Recv 한번만, 끝났으니 다시 걸어주기
	AsyncRecv();
}

void Session::OnSendCompleted(uint32_t transferred)
{
	mSendEvent.owner = nullptr;

	if (transferred == 0)
	{
		DisConnect("Send Zero Byte");
		return;
	}

	// 자식 session 클래스에서 할 작업
	OnSend(transferred);

	{
		WRITE_LOCK;

		mSendBuffer->Remove(transferred);

		mSendRegistered.store(false);
	}
}

bool Session::Connect()
{
	return asyncConnect();
}

void Session::DisConnect(const char* reason)
{
	// 이미 Disconnect 상태면 그냥 리턴
	if (mConnected.exchange(false) == false)
	{
		return;
	}

	//TODO : reason으로 기록 남기기
	cout << reason << endl;

	asyncDisconnect();
}

void Session::Send(const char* buffer, uint32_t contentSize)
{
	if (!mConnected.load())
		return;


	bool registerSend = false;

	{
		WRITE_LOCK;

		if (mSendBuffer->GetFreeSpaceSize() < contentSize)
			return;

		char* destData = mSendBuffer->GetBuffer();
		memcpy(destData, buffer, contentSize);

		mSendBuffer->Commit(contentSize);

		if (mSendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		AsyncSend();
}

void PacketSession::Send(uint16_t packetId, google::protobuf::MessageLite& packet)
{
	if (mConnected.load() == false)
		return;

	bool registerSend = false;

	{
		WRITE_LOCK;


		auto contentSize = packet.ByteSizeLong();

		PacketHeader header;
		header.size = contentSize;
		header.id = packetId;
		header.tickCount = GetTickCount64();

		auto packetSize = sizeof(PacketHeader) + contentSize;

		if (mSendBuffer->GetFreeSpaceSize() < packetSize)
			return;

		google::protobuf::io::ArrayOutputStream arrayOutputStream(mSendBuffer->GetBuffer(), packetSize);
		google::protobuf::io::CodedOutputStream codedOutputStream(&arrayOutputStream);


		codedOutputStream.WriteRaw(&header, sizeof(PacketHeader));
		packet.SerializeToCodedStream(&codedOutputStream);

		mSendBuffer->Commit(packetSize);


		if (mSendRegistered.exchange(true) == false)
			registerSend = true;
	}


	if (registerSend)
		AsyncSend();
}

bool PacketSession::OnRecv()
{
	//패킷분석
	//완료 안되면 다시
	google::protobuf::io::ArrayInputStream arrayInputStream(mRecvBuff->GetBufferStart(), mRecvBuff->GetContiguiousBytes());
	google::protobuf::io::CodedInputStream codedInputStream(&arrayInputStream);

	PacketHeader packetheader;

	while (codedInputStream.ReadRaw(&packetheader, sizeof(PacketHeader)))
	{
		const void* payloadPos = nullptr;
		int payloadSize = 0;

		codedInputStream.GetDirectBufferPointer(&payloadPos, &payloadSize);

		if (payloadSize < packetheader.size) ///< 패킷 본체 사이즈 체크
			return false;

		/// payload 읽기
		google::protobuf::io::ArrayInputStream payloadArrayStream(payloadPos, packetheader.size);
		google::protobuf::io::CodedInputStream payloadInputStream(&payloadArrayStream);


		OnRecvPacket(packetheader, payloadInputStream);

		/// 읽은 만큼 전진 및 버퍼에서 제거
		codedInputStream.Skip(packetheader.size); ///< readraw에서 헤더 크기만큼 미리 전진했기때문
		mRecvBuff->Remove(sizeof(PacketHeader) + packetheader.size);
	}

	return true;
}
