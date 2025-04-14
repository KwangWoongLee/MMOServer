#pragma once
#include "stdafx.h"
#include "SocketUtil.h"
#include "IOCP.h"




class IOCPSession
    : public IIOCPObject
{
public:
	void Dispatch(std::shared_ptr<Overlapped> const iocpEvent, uint32_t const numOfBytes = 0) override;

public:
	IOCPSession();
	virtual ~IOCPSession();

public:
	bool SetSockAddr();
	
	void AsyncRecv();
	void AsyncSend();
	
	void OnAcceptCompleted();
	void OnConnectCompleted();
	void OnDisConnectCompleted();
	void OnRecvCompleted(uint32_t transferred);
	void OnSendCompleted(uint32_t transferred);

	bool Connect();
	void DisConnect(const char* reason);
	void Send(const char* buffer, uint32_t contentSize);

	inline void HandleError(int32_t errorCode)
	{
		switch (errorCode)
		{
		case WSAECONNRESET:
		case WSAECONNABORTED:
			DisConnect("Handle Error");
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
	std::atomic<bool> mConnected = false;
	std::atomic<bool> mSendRegistered = false;

	CircularBufferRef	mRecvBuff;
	CircularBufferRef	mSendBuffer;

	uint16_t			mSendPendingCount = 0;
private:
	SocketAddress mSockAddress;

	RecvEvent				mRecvEvent;
	SendEvent				mSendEvent;
	ConnectEvent			mConnectEvent;
	DisconnectEvent			mDisconnectEvent;

	bool asyncConnect();
	void asyncDisconnect();
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


