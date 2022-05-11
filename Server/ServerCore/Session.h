#pragma once
#include "stdafx.h"

constexpr int HEADER_SIZE = 2;

class Session //추상 클래스
{
public:
	Session() {};
	virtual ~Session() {};

	void Init(SOCKET& socket) noexcept;
	//void Disconnect();
	//void Send();
	//void OnRecvCompleted();
	//void OnSendCompleted();

protected:
	//virtual void OnConnected() = 0;
	//virtual void OnRecv() = 0;
	//virtual void OnSend() = 0;
	//virtual void OnDisconnected() = 0;

private:
	SOCKET mSocket;
	std::atomic<bool> mDisconnect = false;
};

class PacketSession : public Session
{

public:
	PacketSession() {};
	virtual ~PacketSession() {};
private:

};