#pragma once
#include "stdafx.h"
#include "SocketUtil.h"
#include "CircularBuffer.h"
#include "IOContext.h"
#include "IOCP.h"
#include "Engine.h"

class Session : public IOCPObject
{
	friend class Engine;

	//�߻�Ŭ���� IOCPObject ���������Լ� �������̵�
public:
	virtual HANDLE GetHandle() override { return reinterpret_cast<HANDLE>(mSocket); }
	virtual void Dispatch(class Overlapped* iocpEvent, uint32 numOfBytes = 0) override;

public:
	Session();
	virtual ~Session();

public:
	bool SetSockAddr();
	
	void AsyncRecv();
	void AsyncSend();
	
	// �� �Լ��� �ϴ� ������ �����ϳ�, �������� Accept�� �Ǿ��� ����, Ŭ���̾�Ʈ �ʿ��� Connect �Ǿ��� �� ���� ȣ��ȴ�.
	// ������ Ŭ���̾�Ʈ�� �� ���� �����Ƿ� ConnectEx�� �����ؼ� ���
	void OnAcceptCompleted();
	void OnConnectCompleted();
	void OnDisConnectCompleted();
	void OnRecvCompleted(uint32 transferred);
	void OnSendCompleted(uint32 transferred);

	bool Connect();
	void DisConnect(const char* reason);
	void Send(const char* buffer, uint32 contentSize);

	EngineRef GetEngine() { return mEngine.lock(); };
	void SetEngine(EngineRef engine) { mEngine = engine; };

	inline void HandleError(int32 errorCode)
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


	// �ڽ�Ŭ�������� ������ �۾�
	virtual void OnConnected() = 0;
	virtual bool OnRecv() = 0;
	virtual void OnSend(uint32 transferred) = 0;
	virtual void OnDisconnected() = 0;

	char	mAcceptBuf[64] = {};

	USE_LOCK;
	std::atomic<bool> mConnected = false;
	std::atomic<bool> mSendRegistered = false;

	CircularBufferRef	mRecvBuff;
	CircularBufferRef	mSendBuffer;

	uint16			mSendPendingCount = 0;
private:
	
	SOCKET mSocket = INVALID_SOCKET;
	SocketAddress mSockAddress;

	RecvEvent				mRecvEvent;
	SendEvent				mSendEvent;
	ConnectEvent			mConnectEvent;
	DisconnectEvent			mDisconnectEvent;

	std::weak_ptr<Engine>	mEngine;

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

	void Send(uint16 packetId, google::protobuf::MessageLite& packet);

protected:
	virtual bool OnRecv() final override;
	virtual void OnRecvPacket(PacketHeader header, google::protobuf::io::CodedInputStream& payloadInputStream) = 0;

private:
};


