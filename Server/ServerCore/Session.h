#pragma once
#include "stdafx.h"

constexpr int DATA_SIZE = 2;
constexpr size_t BUFF_SIZE = 1024;
constexpr size_t SEND_BUFF_SIZE = 4096;

class Session //추상 클래스
{
public:
	friend class IOCP;

	Session()
		:mRecvBuff(1024)
	{
		mSendBuffer = new char[SEND_BUFF_SIZE];
	};
	virtual ~Session() {};

	void Init(const SOCKET& socket);
	
	size_t GetKey() const { return mSessionKey; };
	void SetKey(size_t key) { mSessionKey = key; };
	
	void AsyncDisconnect(eDisConnectReason reason);
	void AsyncRecv();
	void AsyncSend();
	
	void OnAcceptCompleted();
	void OnRecvCompleted(size_t transferred);
	void OnSendCompleted(size_t transferred);

	void Send(Packet* packet);

	RecvBuffer& GetRecvBuffer() { return mRecvBuff; };

protected:
	virtual void OnConnected() = 0;
	virtual int OnRecv() = 0;
	virtual void OnSend(size_t numOfBytes) = 0;
	virtual void OnDisconnected(eDisConnectReason reason) = 0;


private:
	SOCKET mSocket = INVALID_SOCKET;
	size_t mSessionKey{0};
	int mDisconnected{1};
	RecvBuffer mRecvBuff;
	char*			mSendBuffer;

	std::queue<Packet*> mSendQueue;
	std::mutex		mSendQueueLock;
};

class PacketSession : public Session
{

public:
	PacketSession() {
	};
	virtual ~PacketSession() {};


protected:
	virtual int OnRecv() override;
	virtual void OnSend(size_t numOfBytes) override;
	virtual void OnConnected() override;
	virtual void OnDisconnected(eDisConnectReason reason) override;

	virtual void OnRecvPacket(const char* buffer) = 0;

private:

};

class ClientSession : public PacketSession
{
public:
	ClientSession() {};
	virtual ~ClientSession() {};

	virtual void OnRecvPacket(const char* buffer) override;

};


