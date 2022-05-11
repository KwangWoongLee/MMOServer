#pragma once


enum class eIOType
{
	SEND,
	RECV,
	ACCEPT,
	CONNECT,
	DISCONNECT
};

class Overlapped
{
public:
	WSAOVERLAPPED mOverlapped;

	eIOType mType;
	WSABUF mWSABuf;
}; 



class IOContext
{
};