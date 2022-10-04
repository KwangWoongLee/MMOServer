#pragma once
#include "stdafx.h"

enum class eIOType
{
	SEND,
	RECV,
	ACCEPT,
	CONNECT,
	DISCONNECT
};

class Session;
using SessionPtr = Session*;

enum class eDisConnectReason
{
	UNKNOWN,
	TRANS_ZERO,
	CLOSED_BY_CLIENT,
	RECV_SOCKET_DEALLOC,
	RECV_COMPLETE_ERROR,
	SEND_SOCKET_DEALLOC,
	SEND_COMPLETE_ERROR,
	IO_COMPLETE_ERROR,
};

class Overlapped
{
public:
	Overlapped() {
		memset(&this->mOverlapped, 0, sizeof(WSAOVERLAPPED));

		this->mWSABuf.buf = nullptr;
		this->mWSABuf.len = 0;
	};
	virtual ~Overlapped() {};

	WSAOVERLAPPED mOverlapped;

	eIOType mType;
	WSABUF mWSABuf;

	SessionPtr GetSession() const { return mSession; };
	void SetSession(SessionPtr session) { mSession = session; };


protected:
	SessionPtr mSession = nullptr;
}; 

class AcceptOverlapped : public Overlapped
{
public:
	AcceptOverlapped() {
		mType = eIOType::ACCEPT;
	};

};

class DisConnectOverlapped : public Overlapped
{
public:
	DisConnectOverlapped():mDisConnectReason(eDisConnectReason::UNKNOWN)
	{
		mType = eIOType::DISCONNECT;
	};

	eDisConnectReason mDisConnectReason;
};

class RecvOverlapped : public Overlapped
{
public:
	RecvOverlapped() {
		mType = eIOType::RECV;
	};
};

class SendOverlapped : public Overlapped
{
public:
	SendOverlapped() {
		mType = eIOType::SEND;
	};


};

class IOContext
{

};
