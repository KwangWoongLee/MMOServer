#include "stdafx.h"

#include "IOCPSession.h"
#include "IOCP.h"


void IOCPSession::Dispatch(std::shared_ptr<Overlapped> const iocpEvent, uint32_t const numOfBytes)
{
	switch (iocpEvent->GetIOType())
	{
	case EIOType::ACCEPT:
		OnAcceptCompleted();
		break;
	case EIOType::CONNECT:
		OnConnectCompleted();
		break;
	case EIOType::DISCONNECT:
		OnDisConnectCompleted();
		break;
	case EIOType::SEND:
		OnSendCompleted(numOfBytes);
		break;
	case EIOType::RECV:
		OnRecvCompleted(numOfBytes);
		break;
	
	default:
		break;
	}
}

IOCPSession::IOCPSession()
{
	mSendBuffer = ObjectPool<CircularBuffer>::MakeShared(65535);
	mRecvBuff = ObjectPool<CircularBuffer>::MakeShared(65535);
}

IOCPSession::~IOCPSession()
{
	mSendBuffer = nullptr;
	mRecvBuff = nullptr;
}


bool IOCPSession::SetSockAddr()
{
	SocketAddress sockAddress;
	auto name = reinterpret_cast<SOCKADDR*>(&sockAddress.GetSockAddr());
	int nameSize = sizeof(SOCKADDR);

	if (not ::getpeername(reinterpret_cast<SOCKET>(*GetHandle()), name, &nameSize))
	{
		return false;
	}
	
	_sockAddress = sockAddress;

	return true;
}

bool IOCPSession::asyncConnect()
{
	if (mConnected.load() == true)
		return false;


	auto const& socket = reinterpret_cast<SOCKET>(*GetHandle());
	if (not SocketUtil::Singleton::Instance().Bind(socket))
	{
		return false;
	}

	if (not SocketUtil::Singleton::Instance().SetReuseAddress(socket, true))
	{
		return false;
	}
	

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

void IOCPSession::asyncDisconnect()
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

void IOCPSession::setConnected()
{
	_state = EIOCPSessionState::CONNECTED;

	OnConnected();
	AsyncRecv();
}

void IOCPSession::setDisconnected()
{
	if (EIOCPSessionState::DISCONNECTED == _state)
	{
		return;
	}

	_state = EIOCPSessionState::DISCONNECTED;
	OnDisconnected();
	
	
}

void IOCPSession::AsyncRecv()
{
	if (EIOCPSessionState::CONNECTED == _state)
	{
		return;
	}

	auto const ioEvent = ObjectPool<Overlapped>::Singleton::Instance().Acquire();
	ioEvent->SetIOType(EIOType::RECV);

	ioEvent->Init();
	ioEvent->SetIOCPObject(shared_from_this());
	
	WSABUF wsaBuf;
	wsaBuf.buf = mRecvBuff->GetBuffer();
	wsaBuf.len = static_cast<ULONG>(mRecvBuff->GetFreeSpaceSize());

	DWORD flags = 0;
	DWORD recvBytes = 0;

	auto error = WSARecv(reinterpret_cast<SOCKET>(*GetHandle()), &wsaBuf, 1, &recvBytes, &flags, &(*ioEvent), NULL);
	if (error == SOCKET_ERROR)
	{
		if (auto const err = WSAGetLastError(); WSA_IO_PENDING != err)
		{
			HandleError(err);
		}
	}
}

void IOCPSession::AsyncSend()
{
	if (EIOCPSessionState::CONNECTED != _state)
	{
		return;
	}

	if (_isSendPending.exchange(true))
	{
		return;
	}

	{
		WRITE_LOCK;

		if (0 == mSendBuffer->GetContiguiousBytes())
		{
			return;			
		}

		auto const ioEvent = ObjectPool<Overlapped>::Singleton::Instance().Acquire();
		ioEvent->SetIOType(EIOType::SEND);

		ioEvent->Init();
		ioEvent->SetIOCPObject(shared_from_this());

		DWORD sendbytes = 0;
		DWORD flags = 0;
		WSABUF wsaBuf;
		wsaBuf.len = (ULONG)mSendBuffer->GetContiguiousBytes();
		wsaBuf.buf = mSendBuffer->GetBufferStart();

		if (SOCKET_ERROR == WSASend(reinterpret_cast<SOCKET>(*GetHandle()), &wsaBuf, 1, &sendbytes, flags, &(*ioEvent), NULL))
		{
			if (auto const err = WSAGetLastError(); WSA_IO_PENDING != err)
			{
				HandleError(err);
			}
		}
	}
}

void IOCPSession::OnAcceptCompleted()
{
	setConnected();
}

void IOCPSession::OnConnectCompleted()
{
	setConnected();
}

void IOCPSession::OnDisconnectCompleted()
{
	setDisconnected();
}

void IOCPSession::OnRecvCompleted(uint32_t transferred)
{

	if (transferred == 0)
	{
		Disconnect("Transfered Byte Zero By Client");
		return;
	}

	uint32_t freeSize = mRecvBuff->GetFreeSpaceSize();
	if (transferred > freeSize)
	{
		Disconnect("Recv Buffer Over Write");
		return;
	}

	mRecvBuff->Commit(transferred);

	OnRecv();

	AsyncRecv();
}

void IOCPSession::OnSendCompleted(uint32_t transferred)
{
	mSendEvent.owner = nullptr;

	if (transferred == 0)
	{
		DisConnect("Send Zero Byte");
		return;
	}

	OnSend(transferred);

	{
		WRITE_LOCK;

		mSendBuffer->Remove(transferred);

		mSendRegistered.store(false);
	}
}

bool IOCPSession::Connect()
{
	return asyncConnect();
}

void IOCPSession::Disconnect(const char* reason)
{
	if (mConnected.exchange(false) == false)
	{
		return;
	}

	cout << reason << endl;

	asyncDisconnect();
}

void IOCPSession::Send(const char* buffer, uint32_t contentSize)
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

void PacketIOCPSession::Send(uint16_t packetId, google::protobuf::MessageLite& packet)
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

bool PacketIOCPSession::OnRecv()
{
	google::protobuf::io::ArrayInputStream arrayInputStream(mRecvBuff->GetBufferStart(), mRecvBuff->GetContiguiousBytes());
	google::protobuf::io::CodedInputStream codedInputStream(&arrayInputStream);

	PacketHeader packetheader;

	while (codedInputStream.ReadRaw(&packetheader, sizeof(PacketHeader)))
	{
		const void* payloadPos = nullptr;
		int payloadSize = 0;

		codedInputStream.GetDirectBufferPointer(&payloadPos, &payloadSize);

		if (payloadSize < packetheader.size)
			return false;

		google::protobuf::io::ArrayInputStream payloadArrayStream(payloadPos, packetheader.size);
		google::protobuf::io::CodedInputStream payloadInputStream(&payloadArrayStream);


		OnRecvPacket(packetheader, payloadInputStream);

		codedInputStream.Skip(packetheader.size); 
		mRecvBuff->Remove(sizeof(PacketHeader) + packetheader.size);
	}

	return true;
}
