#include "stdafx.h"
#include "Listener.h"

void Listener::Init(short port)
{
	WSADATA WSAData;
	SOCKADDR_IN sockAddr;

	try {
		if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
			throw std::format("Winsock Init Failed {}", WSAGetLastError());

		mListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (mListenSocket == INVALID_SOCKET)
			throw std::format("Listen Socket Create Failed {}", WSAGetLastError());

		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port);

		if (bind(mListenSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)))
			throw std::format("Listen Socket Bind Failed {}", WSAGetLastError());
		if (SOCKET_ERROR == listen(mListenSocket, SOMAXCONN))
			throw std::format("Listen Failed {}", WSAGetLastError());

	}
	catch (std::string& e)
	{
		if (mListenSocket != NULL)
		{
			closesocket(mListenSocket);
			mListenSocket = NULL;
		}

		throw e;
	}
}
