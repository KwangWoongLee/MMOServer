#include "stdafx.h"
#include "IOCP.h"
#include "IOContext.h"

void IOCP::createCompletionPort()
{
	// ���ʿ� �ѹ��� CompletionPort �ʱ�ȭ
	mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (mCompletionPort == NULL)
		throw std::format("IOCP Create Failed {}", GetLastError());
}

void IOCP::RegistCompletionPort(SOCKET socket, ULONG_PTR key)
{
	assert(socket != INVALID_SOCKET);

	// Accept�� ���� ��, CompletionPort�� ���
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
	// PostQueuedCompletionStatus �� IOCP�� ���� ��ȣ�� ��
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
				// �����ϸ�  TRUE�� ��ȯ�ϰ� �׷��� ������ FALSE�� ��ȯ�մϴ�.
		
		if (ret == true) // ����
		{
			//if(mCompletionPort == THREAD_DIE)

			if (mCompletionPort == THREAD_DIE) // mCompletionPort == 0�� ���, Post�� �������� ���� �޽��� ���� �� => ������ ����
			{
				break;
			}

			if (overlapped == nullptr)
			{
				// �Ϸ� ��Ŷ �������� ���� �α� �����
				continue;
			}

		}
		else { // ����
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

		//������� ����, eIOType �� �°� ����ó��
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
					//Ŭ���̾�Ʈ�� ���� ���
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
					//Ŭ���̾�Ʈ�� ���� ���
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
			//Ŭ���̾�Ʈ �����ϱ�
			clientSession->AsyncDisconnect(reason);
		}

		//delete overlapped;
		
	}
}
