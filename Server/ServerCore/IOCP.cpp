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
	assert(threadCount >= 0);

	for (int i = 0; i < threadCount; ++i)
		mIOWorkerThreadPool.push_back(std::move(std::thread(&IOCP::IOWorkerFunc, this)));
}

void IOCP::Start(short threadCount)
{
	createCompletionPort();
	createThreadPool(threadCount);
}

void IOCP::Stop()
{
	stopThreads();
}

void IOCP::stopThreads()
{
	// PostQueuedCompletionStatus 로 IOCP에 종료 신호를 줌
	BOOL ret = PostQueuedCompletionStatus(mCompletionPort, 0, THREAD_DIE, NULL);

	if (!ret)
		throw std::format("Stop IOCP Failed {}", GetLastError());

	//for (auto& thread : mIOWorkerThreadPool)
	//{
	//	if(thread.joinable())
	//		thread.join();
	//}
}


void IOCP::IOWorkerFunc()
{
	//threadType = eThreadType::IO_WORKER;

	while (true)
	{
		Overlapped* overlapped = nullptr;
		SessionPtr clientSession = nullptr;
		DWORD dwTransferred = 0; 


		bool ret = GetQueuedCompletionStatus(mCompletionPort, &dwTransferred, reinterpret_cast<PULONG_PTR>(&clientSession), reinterpret_cast<LPOVERLAPPED*>(&overlapped), INFINITE);
				// 성공하면  TRUE를 반환하고 그렇지 않으면 FALSE를 반환합니다.
		
		if (ret == true) // 성공
		{
			//if(mCompletionPort == THREAD_DIE)

			if (mCompletionPort == THREAD_DIE) // mCompletionPort == 0인 경우, Post로 서버에서 종료 메시지 보낸 것 => 쓰레드 종료
			{
				break;
			}

			if (overlapped == nullptr)
			{
				// 완료 패킷 가져오기 실패 로그 남기기
				continue;
			}

		}
		else { // 실패
			if (overlapped == nullptr)
			{
				if (WSAGetLastError() == WAIT_TIMEOUT)
					return;
			}
			else 
			{
				clientSession->AsyncDisconnect(eDisConnectReason::IO_COMPLETE_ERROR);
			}

			continue;
		}

		//여기까지 오면, eIOType 에 맞게 정상처리
		bool except = false;

		try 
		{
			switch (auto ioType = overlapped->mType; ioType)
			{
			case eIOType::ACCEPT:
			{
				auto acceptOverlapped = static_cast<AcceptOverlapped*>(overlapped);
				auto session = acceptOverlapped->GetSession();

				session->OnAcceptCompleted();
			}
				break;

			case eIOType::DISCONNECT:
			{
				auto disConnectOverlapped = static_cast<DisConnectOverlapped*>(overlapped);
				clientSession->OnDisconnected(disConnectOverlapped->mDisConnectReason);
			}
				break;

			case eIOType::SEND:
			{
				if (dwTransferred == 0)
				{
					//클라이언트가 끊은 경우
					throw eDisConnectReason::TRANS_ZERO;
				}


				if (overlapped == nullptr)
				{
					throw eDisConnectReason::SEND_COMPLETE_ERROR;
				}

				//TODO
				clientSession->OnSendCompleted(dwTransferred);
			}
				break;

			case eIOType::RECV:
			{
				if (dwTransferred == 0)
				{
					//클라이언트가 끊은 경우
					throw eDisConnectReason::TRANS_ZERO;
				}

				if (overlapped == nullptr)
				{
					throw eDisConnectReason::RECV_COMPLETE_ERROR;
				}

				clientSession->OnRecvCompleted(dwTransferred);
			}
				break;

			default:
				throw eDisConnectReason::UNKNOWN;
				break;

			}
			
		}
		catch (eDisConnectReason reason)
		{
			//클라이언트 종료하기
			clientSession->AsyncDisconnect(reason);
		}

		//delete overlapped;
		
	}
}
