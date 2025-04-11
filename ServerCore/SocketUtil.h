#pragma once

class SocketAddress
{
public:
	SocketAddress();
	virtual ~SocketAddress() = default;

	SocketAddress(uint16_t port);
	SocketAddress(std::string_view ip, uint16_t port);

public:
	SOCKADDR_IN& GetSockAddr() { return mSockAddr; }


private:
	SOCKADDR_IN mSockAddr;

};



extern LPFN_CONNECTEX		FnConnectEx;
extern LPFN_DISCONNECTEX	FnDisconnectEx;
extern LPFN_ACCEPTEX		FnAcceptEx;

class SocketUtil
{
public:
    using Singleton = Singleton<SocketUtil>;

public:
	SocketUtil() = default;
	virtual ~SocketUtil() {
		::WSACleanup();
	};

public:
	bool Init();

	SOCKET CreateSocket();

	// Socket 옵션
	bool	SetLinger(SOCKET socket, uint16_t onoff, uint16_t linger);
	bool	SetReuseAddress(SOCKET socket, bool flag);
	bool	SetRecvBufferSize(SOCKET socket, int32_t size);
	bool	SetSendBufferSize(SOCKET socket, int32_t size);
	bool	SetTcpNoDelay(SOCKET socket, bool flag);
	bool	SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

private:
	// Ex 함수 Set
	bool	setExFunction();

};