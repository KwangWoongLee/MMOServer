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

class Overlapped : public OVERLAPPED
{
public:
	Overlapped() { Init(); };
	virtual ~Overlapped() = default;
	inline void Init() {
		OVERLAPPED::hEvent = 0;
		OVERLAPPED::Internal = 0;
		OVERLAPPED::InternalHigh = 0;
		OVERLAPPED::Offset = 0;
		OVERLAPPED::OffsetHigh = 0;
	};

	IOCPObjectRef GetOwner() const { return mOwner; };
	void	SetOwner(IOCPObjectRef iocpObject) { mOwner = iocpObject; };

	eIOType GetType() const { return mType; };
	void SetType(eIOType type) { mType = type; };

protected:

private:
	IOCPObjectRef mOwner = nullptr;
	eIOType mType;
};


#pragma region Overlapped For Listener

class AcceptEvent : public Overlapped
{
public:
	AcceptEvent() : Overlapped() {
		SetType(eIOType::ACCEPT);
	};

	SessionRef GetSession() { return mSession; };
	void SetSession(SessionRef session) { mSession = session; }

private:
	SessionRef mSession = nullptr;
};


#pragma endregion

#pragma region Overlapped For Session

class ConnectEvent : public Overlapped
{
public:
	ConnectEvent()
	{
		SetType(eIOType::CONNECT);
	};
};


class DisconnectEvent : public Overlapped
{
public:
	DisconnectEvent()
	{
		SetType(eIOType::DISCONNECT);
	};
};

class RecvEvent : public Overlapped
{
public:
	RecvEvent() {
		SetType(eIOType::RECV);
	};
};

class SendEvent : public Overlapped
{
public:
	SendEvent() {
		SetType(eIOType::SEND);
	};

	//Vector<SendBufferRef>& GetSendBuffers() { return sendBuffers; };

private:
	//Vector<SendBufferRef> sendBuffers;
};

#pragma endregion
