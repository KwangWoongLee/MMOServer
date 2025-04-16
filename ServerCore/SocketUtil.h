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

	SOCKET CreateSocket() const;
	void CloseSocket(SOCKET const& socket) const;
	bool Bind(SOCKET const& socket) const;
	bool Listen(SOCKET const& socket, int32_t const backlog = SOMAXCONN) const;

	bool	SetLinger(SOCKET const& socket, uint16_t const onoff, uint16_t const linger) const;
	bool	SetReuseAddress(SOCKET const& socket, bool const flag) const;
	bool	SetRecvBufferSize(SOCKET const& socket, int32_t const size) const;
	bool	SetSendBufferSize(SOCKET const& socket, int32_t const size) const;
	bool	SetTcpNoDelay(SOCKET const& socket, bool const flag) const;
	bool	SetUpdateAcceptSocket(SOCKET const& socket, SOCKET const listenSocket) const;

private:
	bool	setExFunction();

	template<typename T>
	static inline bool setSockOpt(SOCKET const& socket, int32_t const level, int32_t const optName, T const& optVal)
	{
		return SOCKET_ERROR != ::setSockOpt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
	}
};