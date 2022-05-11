#include "stdafx.h"
#include "Listener.h"
#include "IOCP.h"
#include "IOContext.h"

void Listener::Init(short port)
{
	WSADATA WSAData;
	SOCKADDR_IN sockAddr;

	try {
		if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
			throw std::format("Winsock Init Failed {}", WSAGetLastError());

		mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (mListenSocket == INVALID_SOCKET)
			throw std::format("Listen Socket Create Failed {}", WSAGetLastError());

		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(port);

		if (bind(mListenSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)))
			throw std::format("Listen Socket Bind Failed {}", WSAGetLastError());

		if (SOCKET_ERROR == listen(mListenSocket, SOMAXCONN))
			throw std::format("Listen Failed {}", WSAGetLastError());

		IOCP::GetInstance().RegistCompletionPort(mListenSocket,(ULONG_PTR)0);

		//TODO : Set Socket Option
		 

		//AcceptEx Register
		DWORD bytes = 0;

		GUID guidAcceptEx = WSAID_ACCEPTEX;
		if (SOCKET_ERROR == WSAIoctl(mListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidAcceptEx, sizeof(GUID), &FnAcceptEx, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL))
			throw std::format("AcceptEx Regist Failed {}", WSAGetLastError());

		
	}
	catch (std::string& e)
	{
		if (mListenSocket != NULL)
		{
			closesocket(mListenSocket);
			mListenSocket = NULL;
		}

		WSACleanup();

		throw e;
	}
}

void Listener::Start()
{
	asyncAccept([]() { return std::make_shared<PacketSession>(); });
}

void Listener::Stop()
{
	if (mListenSocket != NULL)
	{
		closesocket(mListenSocket);
		mListenSocket = NULL;
	}
	
	WSACleanup();
}

void Listener::asyncAccept(std::function<std::shared_ptr<Session>()> sessionFactory)
{
	DWORD bytes = 0;

	Overlapped* acceptOverlapped = new Overlapped;
	memset(&acceptOverlapped->mOverlapped, 0, sizeof(WSAOVERLAPPED));

	acceptOverlapped->mType = eIOType::ACCEPT;
	acceptOverlapped->mWSABuf.buf = nullptr;
	acceptOverlapped->mWSABuf.len = 0;


	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET)
		throw std::format("Accept Socket Create Failed {}", WSAGetLastError());

	auto session = sessionFactory();
	session->Init(clientSocket);

	IOCP::GetInstance().RegistCompletionPort(clientSocket, reinterpret_cast<ULONG_PTR>(session.get()));

	if (FALSE == FnAcceptEx(mListenSocket, clientSocket, acceptBuff, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPOVERLAPPED)acceptOverlapped))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// Pending 외에 다른 오류라면 잘못된것
			delete acceptOverlapped;
			std::cout << WSAGetLastError();
			//throw std::format("Async Accept Failed {}", WSAGetLastError());
		}
	}
}

