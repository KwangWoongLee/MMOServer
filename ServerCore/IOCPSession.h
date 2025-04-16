#pragma once
#include "stdafx.h"
#include "SocketUtil.h"
#include "IOCP.h"
#include "IOEvent.h"

namespace
{
	enum class EIOCPSessionState : uint8_t
	{
		NONE,
		CONNECTING,
		CONNECTED,
		DISCONNECTING,
		DISCONNECTED
	};
}

class IOCPSession
    : public IIOCPObject
{
public:
	IOCPSession();
	virtual ~IOCPSession();

public:
	void Dispatch(std::shared_ptr<Overlapped> const iocpEvent, uint32_t const numOfBytes = 0) override;

	bool SetSockAddr();
	
	void AsyncRecv();
	void AsyncSend();
	
	void OnAcceptCompleted();
	void OnConnectCompleted();
	void OnDisconnectCompleted();
	void OnRecvCompleted(uint32_t const transferred);
	void OnSendCompleted(uint32_t const transferred);

	bool Connect();
	void Disconnect(const char* reason);
	void Send(const char* buffer, uint32_t contentSize);

	inline void HandleError(int32_t const errorCode)
	{
		switch (errorCode)
		{
		case WSAECONNRESET:
		case WSAECONNABORTED:
			Disconnect("Handle Error");
			break;
		default:
			cout << "Handle Error : " << errorCode << endl;
			break;
		}
	}


	virtual void OnConnected() = 0;
	virtual bool OnRecv() = 0;
	virtual void OnSend(uint32_t transferred) = 0;
	virtual void OnDisconnected() = 0;

	char	mAcceptBuf[64] = {};

	USE_LOCK;

	CircularBufferRef	mRecvBuff;
	CircularBufferRef	mSendBuffer;

private:
	SocketAddress _sockAddress;

	std::atomic<EIOCPSessionState> _state{ EIOCPSessionState::NONE };
	std::atomic<bool> _isSendPending{};

	bool asyncConnect();
	void asyncDisconnect();

	void setConnected();
	void setDisconnected();
};

class PacketSession : public Session
{

public:
	PacketSession() {
	};
	virtual ~PacketSession() {};

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); };

	void Send(uint16_t packetId, google::protobuf::MessageLite& packet);

protected:
	virtual bool OnRecv() final override;
	virtual void OnRecvPacket(PacketHeader header, google::protobuf::io::CodedInputStream& payloadInputStream) = 0;

private:
};


