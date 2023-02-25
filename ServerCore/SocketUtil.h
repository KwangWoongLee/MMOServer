#pragma once

class SocketAddress
{
public:
	SocketAddress();
	virtual ~SocketAddress() = default;

	SocketAddress(std::string_view ip, uint32 port);

public:
	SOCKADDR_IN& GetSockAddr() { return mSockAddr; }


private:
	SOCKADDR_IN mSockAddr;

};



extern LPFN_CONNECTEX		FnConnectEx;
extern LPFN_DISCONNECTEX	FnDisconnectEx;
extern LPFN_ACCEPTEX		FnAcceptEx;

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

	// Socket �ɼ�
	bool	SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	bool	SetReuseAddress(SOCKET socket, bool flag);
	bool	SetRecvBufferSize(SOCKET socket, int32 size);
	bool	SetSendBufferSize(SOCKET socket, int32 size);
	bool	SetTcpNoDelay(SOCKET socket, bool flag);
	bool	SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

private:
	// Ex �Լ� Set
	bool	setExFunction();

};