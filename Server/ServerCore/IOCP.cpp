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
	// PostQueuedCompletionStatus �� IOCP�� ���� ��ȣ�� ��
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
				// �����ϸ� 0�� �ƴ� ��( TRUE )�� ��ȯ�ϰ� �׷��� ������ 0( FALSE )�� ��ȯ�մϴ�.

		if (ret == false) // ����
		{

			continue;
		}
		else { // ����
			if (key == 0) // key == 0�� ���, Post�� �������� ���� �޽��� ���� �� => ������ ����
			{
				break;
			}
			
		}

		eIOType ioType = overlapped->mType;

		//������� ����, IOType�� �°� ����ó��
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
			

			// Ŭ���̾�Ʈ �� ���������� ���
		}
		catch (std::string& e)
		{

		}
		

	}
}
