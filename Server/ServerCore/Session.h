#pragma once
#include "stdafx.h"
#include "SocketUtil.h"
#include "CircularBuffer.h"
#include "IOContext.h"
#include "IOCP.h"

constexpr int DATA_SIZE = 2;
constexpr size_t BUFF_SIZE = 1024;
constexpr size_t SEND_BUFF_SIZE = 4096;


class Session : public IOCPObject
{
	using RecvBuffer = CircularBuffer;
	//�߻�Ŭ���� IOCPObject ���������Լ� �������̵�
public:
	virtual HANDLE GetHandle() const override { return reinterpret_cast<HANDLE>(mSocket); }
	virtual void Dispatch(class Overlapped* iocpEvent, uint32 numOfBytes = 0) = 0;

public:
	Session()
	{
		mSendBuffer = new char[SEND_BUFF_SIZE];
	};
	virtual ~Session() {};

public:
	void Init(const SOCKET& socket);

	bool SetSockAddr();
	
	void AsyncDisconnect(const char* reason);
	void AsyncRecv();
	void AsyncSend();
	
	// �� �Լ��� �ϴ� ������ �����ϳ�, �������� Accept�� �Ǿ��� ����, Ŭ���̾�Ʈ �ʿ��� Connect �Ǿ��� �� ���� ȣ��ȴ�.
	// ������ Ŭ���̾�Ʈ�� �� ���� �����Ƿ� ConnectEx�� �����ؼ� ���
	void OnAcceptCompleted();
	void OnConnectCompleted();


	void OnRecvCompleted(uint32 transferred);
	void OnSendCompleted(uint32 transferred);

	void Send();

	RecvBuffer& GetRecvBuffer() { return mRecvBuff; };

protected:
	virtual void OnConnected() = 0;
	virtual uint32 OnRecv() = 0;
	virtual void OnSend(uint32 numOfBytes) = 0;
	virtual void OnDisconnected(const char* reason) = 0;


private:
	SOCKET mSocket = INVALID_SOCKET;
	SocketAddress mSockAddress;
	std::atomic<bool> mConnected = false;


	RecvBuffer		mRecvBuff;
	char*			mSendBuffer;

	//std::queue<Packet*> mSendQueue;
	//std::mutex		mSendQueueLock;

	RecvEvent				mRecvEvent;
	SendEvent				mSendEvent;
	ConnectEvent			mConnectEvent;
	DisconnectEvent			mDisconnectEvent;
};

class PacketSession : public Session
{

public:
	PacketSession() {
	};
	virtual ~PacketSession() {};


protected:
	virtual uint32 OnRecv() override;
	virtual void OnSend(uint32 numOfBytes) override;
	virtual void OnConnected() override;
	virtual void OnDisconnected(const char* reason) override;

	virtual void OnRecvPacket(const char* buffer, uint16 id, uint16 contentSize) = 0;

private:
};


