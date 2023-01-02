#pragma once

class SocketAddress
{
public:
	SocketAddress();
	virtual ~SocketAddress() = default;

	SocketAddress(const char* ip, uint32 port);

public:
	SOCKADDR& GetSockAddr() { return mSockAddr; }
	sockaddr_in* GetSockAddrInPtr()
	{
		return reinterpret_cast<sockaddr_in*>(&mSockAddr);
	}

private:
	SOCKADDR mSockAddr;

};

class SocketUtil : public Singleton<SocketUtil>
{
public:
	SocketUtil() = default;
	virtual ~SocketUtil() {
		::WSACleanup();
	};

public:
	bool Init();

	SOCKET			CreateSocket();

	// Socket 옵션
	bool	SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	bool	SetReuseAddress(SOCKET socket, bool flag);
	bool	SetRecvBufferSize(SOCKET socket, int32 size);
	bool	SetSendBufferSize(SOCKET socket, int32 size);
	bool	SetTcpNoDelay(SOCKET socket, bool flag);
	bool	SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

private:
	// Ex 함수 Set
	bool	setExFunction();

};

