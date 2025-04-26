#include "stdafx.h"

#include "IOCPSession.h"
#include "IOCP.h"


void IOCPSession::Dispatch(Overlapped const* iocpEvent, uint32_t const numOfBytes)
{
	switch (iocpEvent->GetIOType())
	{
	case EIOType::ACCEPT:
		{
			OnAcceptCompleted();
		}	
		break;
	case EIOType::CONNECT:
		{
			OnConnectCompleted();
		}		
		break;
	case EIOType::DISCONNECT:
		{
			OnDisconnectCompleted();
		}
		break;
	case EIOType::SEND:
		{
			OnSendCompleted(numOfBytes);
		}
		break;
	case EIOType::RECV:
		{
			OnRecvCompleted(numOfBytes);
		}
		break;
	default:
		{}
		break;
	}
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
	if (EIOCPSessionState::CONNECTING != _state)
	{
	}

	auto const& socket = reinterpret_cast<SOCKET>(*GetHandle());
	if (not SocketUtil::Singleton::Instance().Bind(socket))
	{
		return false;
	}

	if (not SocketUtil::Singleton::Instance().SetReuseAddress(socket, true))
	{
		return false;
	}

	auto const connectIOEvent = Overlapped::GetObjectPoolIOEvent(EIOType::CONNECT, shared_from_this());

	DWORD numOfBytes = 0;
	if (not FnConnectEx(reinterpret_cast<SOCKET>(GetHandle()), reinterpret_cast<const SOCKADDR*>(&serverSockAddress.GetSockAddr()), sizeof(serverSockAddress.GetSockAddr()), nullptr, 0, &numOfBytes, connectIOEvent))
	{
        if (const auto error = WSAGetLastError(); WSA_IO_PENDING != error)
		{
			ObjectPool<Overlapped>::Singleton::Instance().Release(connectIOEvent);
			return false;
		}
	}

	return true;
}

void IOCPSession::asyncDisconnect()
{
	auto const disconnectIOEvent = Overlapped::GetObjectPoolIOEvent(EIOType::DISCONNECT, shared_from_this());

	if (not FnDisconnectEx(reinterpret_cast<SOCKET>(GetHandle()), disconnectIOEvent, TF_REUSE_SOCKET, 0))
	{
		if (const auto error = WSAGetLastError(); WSA_IO_PENDING != error)
		{
			ObjectPool<Overlapped>::Singleton::Instance().Release(disconnectIOEvent);
		}
	}
}

void IOCPSession::setConnected()
{
	_state = EIOCPSessionState::CONNECTED;

	OnConnected();
	asyncRecv();
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

void IOCPSession::asyncRecv()
{
	if (EIOCPSessionState::CONNECTED != _state)
	{
		return;
	}

	auto const recvIOEvent = Overlapped::GetObjectPoolIOEvent(EIOType::RECV, shared_from_this());
	
	WSABUF wsaBuf;
	wsaBuf.buf = _recvBuffer.GetBuffer();
	wsaBuf.len = static_cast<ULONG>(_recvBuffer.GetFreeSpaceSize());

	DWORD flags = 0;
	DWORD recvBytes = 0;

	auto error = WSARecv(reinterpret_cast<SOCKET>(*GetHandle()), &wsaBuf, 1, &recvBytes, &flags, recvIOEvent, NULL);
	if (error == SOCKET_ERROR)
	{
		if (auto const err = WSAGetLastError(); WSA_IO_PENDING != err)
		{
			handleError(err);
		}
	}
}

void IOCPSession::asyncSend()
{
	if (EIOCPSessionState::CONNECTED != _state)
	{
		return;
	}

	WRITE_LOCK;

	_isSendPending = true;

	if (0 == _sendBuffer.GetContiguiousBytes())
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
	wsaBuf.len = static_cast<ULONG>(_sendBuffer.GetContiguiousBytes());
	wsaBuf.buf = _sendBuffer.GetBufferStart();

	if (SOCKET_ERROR == WSASend(reinterpret_cast<SOCKET>(*GetHandle()), &wsaBuf, 1, &sendbytes, flags, &(*ioEvent), NULL))
	{
		if (auto const err = WSAGetLastError(); WSA_IO_PENDING != err)
		{
			handleError(err);
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

void IOCPSession::OnRecvCompleted(uint32_t const transferred)
{
	if (0 == transferred)
	{
		Disconnect(EDisconnectReason::RECV_ZERO);
		return;
	}

	uint32_t const freeSize = _recvBuffer.GetFreeSpaceSize();
	if (freeSize < transferred)
	{
		Disconnect(EDisconnectReason::RECV_OVERFLOW);
		return;
	}

	_recvBuffer.Commit(transferred);

	// OnRecv Å¥À×

	asyncRecv();
}

void IOCPSession::OnSendCompleted(uint32_t const transferred)
{
	if (0 == transferred)
	{
		Disconnect(EDisconnectReason::SEND_ZERO);
		return;
	}

	// OnSend Å¥À×

	{
		WRITE_LOCK;

		_sendBuffer.Remove(transferred);

		_isSendPending = false;
	}
}

bool IOCPSession::Connect()
{
	return asyncConnect();
}

void IOCPSession::Disconnect(EDisconnectReason const reason)
{
	if (EIOCPSessionState::CONNECTED != _state)
	{
		return;
	}

	//TODO: LOG
	auto const reasonStr = ToString(reason);

	asyncDisconnect();
}

void IOCPSession::Send(const char* buffer, uint32_t contentSize)
{
	if (EIOCPSessionState::CONNECTED != _state)
	{
		return;
	}

	bool registerSend = false;

	{
		std::scoped_lock lock(_sendMutex);

		if (_sendBuffer.GetFreeSpaceSize() < contentSize)
			return;

		auto* destData = _sendBuffer.GetBuffer();
		memcpy(destData, buffer, contentSize);

		_sendBuffer.Commit(contentSize);

		if (_isSendPending.exchange(true) == false)
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

		if (_sendBuffer.GetFreeSpaceSize() < packetSize)
			return;

		google::protobuf::io::ArrayOutputStream arrayOutputStream(_sendBuffer.GetBuffer(), packetSize);
		google::protobuf::io::CodedOutputStream codedOutputStream(&arrayOutputStream);


		codedOutputStream.WriteRaw(&header, sizeof(PacketHeader));
		packet.SerializeToCodedStream(&codedOutputStream);

		_sendBuffer.Commit(packetSize);


		if (_isSendPending.exchange(true) == false)
			registerSend = true;
	}


	if (registerSend)
		AsyncSend();
}

bool PacketIOCPSession::OnRecv()
{
	google::protobuf::io::ArrayInputStream arrayInputStream(_recvBuffer.GetBufferStart(), _recvBuffer.GetContiguiousBytes());
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
		_recvBuffer.Remove(sizeof(PacketHeader) + packetheader.size);
	}

	return true;
}
