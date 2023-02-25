#include "stdafx.h"

#include "GRPC.h"
#include "ServerEngine.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "ThreadManager.h"
#include "GameManager.h"
#include "RoomManager.h"
#include "MatchManager.h"

#include <Python.h>

void AsyncSendJob()
{
	// TLS �۾��̶� No Lock
	while (TLS_SendSessionQueue.empty() == false)
	{
		auto sessionRef = TLS_SendSessionQueue.front();

		if (sessionRef->AsyncSend() == true) // ���� Send �۾��� ���� ���, ����
		{
			sessionRef = nullptr;
			TLS_SendSessionQueue.pop();
		}
	}
}


int main()
{


	try {
		PacketHandler::Init();

		std::string tcpHost = "127.0.0.1";
		uint16		tcpPort = 7777;

		std::string grpcHost = "localhost";
		uint16		grpcPort = 5001;

		ServerEngineRef server = std::make_shared<ServerEngine>(tcpHost, tcpPort, std::make_unique<IOCP>(), 1000, []() { return std::make_shared<GameSession>(); }
			);
		//	
		if (server->Init() == false)
		{
			server = nullptr;

			throw std::exception("Server Init Failed");
		}
		std::cout << "TCP Server Start !" << std::endl;

		//if (gMatchManager->Init(grpcHost, grpcPort) == false)
		//{
		//		throw std::exception("Match Server Add Failed");
		//}
		//std::cout << "Regist At Match Server !" << std::endl;


		//gThreadManager->AddThread([]()
		//	{
		//		while (true)
		//		{
		//			MatchClient::CreateRoomJobBi();
		//		}
		//	});


		auto newRoom = gRoomManager->Add(1, 1, 10, 1);

		int threadCount = 5;

		gGameManager->DoTimer(250, &GameManager::Update);

		for (auto i = 0; i < threadCount; ++i)
		{
			gThreadManager->AddThread([&server]()
				{
					while (true)
					{
						TLS_LastTickCount = ::GetTickCount64() + 64;

						server->Run(10);

						// ����� �ϰ� ó��
						ThreadManager::DistributeReservedJobs();

						// �۷ι� ť
						ThreadManager::DoGlobalQueueWork();

						AsyncSendJob();
					}
				});

		}

		gThreadManager->Join();
		
			
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1; // ������ ����
	}	

	return 0;
}