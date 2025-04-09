#pragma once
#include "stdafx.h"
#include "IOCP.h"
#include "Session.h"

enum class eIOType : uint8_t
{
	SEND,
	RECV,
	ACCEPT,
	CONNECT,
	DISCONNECT
};

class Overlapped : public OVERLAPPED
{
public:
	Overlapped(eIOType type) 
		: ioType(type)
	{ 		
		Init(); 
	};

	void Init();

public:
	eIOType ioType;
	IOCPObjectRef owner = nullptr;
};

#pragma region Overlapped For Listener

class AcceptEvent : public Overlapped
{
public:
	AcceptEvent() : Overlapped(eIOType::ACCEPT)
	{
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
	ConnectEvent() : Overlapped(eIOType::CONNECT)
	{};
};


class DisconnectEvent : public Overlapped
{
public:
	DisconnectEvent() : Overlapped(eIOType::DISCONNECT)
	{};
};

class RecvEvent : public Overlapped
{
public:
	RecvEvent() :Overlapped(eIOType::RECV)
	{};
};

class SendEvent : public Overlapped
{
public:
	SendEvent() : Overlapped(eIOType::SEND)
	{};
};

#pragma endregion
