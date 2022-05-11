#include "stdafx.h"
#include "IOCP.h"
#include "IOContext.h"

void IOCP::createCompletionPort()
{
	// 최초에 한번만 CompletionPort 초기화
	mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (mCompletionPort == NULL)
		throw std::format("IOCP Create Failed {}", GetLastError());
}

void IOCP::RegistCompletionPort(SOCKET socket, ULONG_PTR key)
{
	assert(socket != INVALID_SOCKET);

	// Accept가 들어올 때, CompletionPort에 등록
	HANDLE handle = CreateIoCompletionPort((HANDLE)socket, mCompletionPort, key, 0);

	if (handle != mCompletionPort)
		throw std::format("IOCP Regist Failed {}", GetLastError());
}

void IOCP::createThreadPool(short threadCount)
{
	assert(threadCount > 0);

	for (int i = 0; i < threadCount; ++i)
		mIOWorkerThreadPool.push_back(std::move(std::thread(&IOCP::IOWorkerFunc, this)));
}

void IOCP::Start()
{
	createCompletionPort();
	createThreadPool(3);
}

void IOCP::Stop()
{
	stopThreads();
}

void IOCP::stopThreads()
{
	// PostQueuedCompletionStatus 로 IOCP에 종료 신호를 줌
	BOOL ret = PostQueuedCompletionStatus(mCompletionPort, 0, NULL, NULL);

	if (!ret)
		throw std::format("Stop IOCP Failed {}", GetLastError());

	for (auto& thread : mIOWorkerThreadPool)
	{
		if(thread.joinable())
			thread.join();
	}
}

void IOCP::IOWorkerFunc()
{
	//threadType = eThreadType::IO_WORKER;

	while (true)
	{
		Overlapped* overlapped = nullptr;
		ULONG_PTR key = 0;
		DWORD dwTransferred = 0;

		int ret = GetQueuedCompletionStatus(mCompletionPort, &dwTransferred, (PULONG_PTR)&key, (LPOVERLAPPED*)&overlapped, INFINITE);
				// 성공하면 0이 아닌 값( TRUE )을 반환하고 그렇지 않으면 0( FALSE )을 반환합니다.

		if (ret == false) // 실패
		{

			continue;
		}
		else { // 성공
			if (key == 0) // key == 0인 경우, Post로 서버에서 종료 메시지 보낸 것 => 쓰레드 종료
			{
				break;
			}
			
		}

		eIOType ioType = overlapped->mType;

		//여기까지 오면, IOType에 맞게 정상처리
		try 
		{
			switch (ioType)
			{
			case eIOType::ACCEPT:
				break;
			case eIOType::CONNECT:
				break;
			case eIOType::DISCONNECT:
				break;
			case eIOType::SEND:
				break;
			case eIOType::RECV:
				break;
			default:
				throw "";
				break;

			
			}
			

			// 클라이언트 쪽 강제종료인 경우
		}
		catch (std::string& e)
		{

		}
		

	}
}
