#include "stdafx.h"
#include "SocketUtil.h"

SocketAddress::SocketAddress()
{
	ZeroMemory(&mSockAddr, sizeof(SOCKADDR));
}

SocketAddress::SocketAddress(const char* ip, uint32 port)
{
	ZeroMemory(&mSockAddr, sizeof(SOCKADDR));

	GetSockAddrInPtr()->sin_family = AF_INET;
	GetSockAddrInPtr()->sin_addr.S_un.S_addr = htonl(::InetPtonA(AF_INET, ip, &mSockAddr));
	GetSockAddrInPtr()->sin_port = htons(port);
}

bool SocketUtil::Init()
{
	WSADATA WSAData;

	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
		return false;

	if (setExFunction() == false)
		return false;

	return true;
}

SOCKET SocketUtil::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}


bool SocketUtil::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtil::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtil::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtil::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtil::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

// ListenSocket의 특성을 ClientSocket에 그대로 적용
bool SocketUtil::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketUtil::setExFunction()
{	//AcceptEx Register
	DWORD bytes = 0;
	SOCKET tmpSocket = this->CreateSocket();

	GUID guidAcceptEx = WSAID_ACCEPTEX;
	if (SOCKET_ERROR == WSAIoctl(tmpSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx, sizeof(GUID), &FnAcceptEx, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL))
		return false;

	GUID guidConnectEx = WSAID_CONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(tmpSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidConnectEx, sizeof(GUID), &FnConnectEx, sizeof(LPFN_CONNECTEX), &bytes, NULL, NULL))
		return false;

	GUID guidDisconnectEx = WSAID_DISCONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(tmpSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidDisconnectEx, sizeof(GUID), &FnDisConnectEx, sizeof(LPFN_DISCONNECTEX), &bytes, NULL, NULL))
		return false;

	::closesocket(tmpSocket);
	tmpSocket = INVALID_SOCKET;

	return true;
}
