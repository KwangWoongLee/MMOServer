#include "stdafx.h"

#include "GRPC.h"
#include "ServerEngine.h"
#include "GameSession.h"
#include "PacketHandler.h"
#include "ThreadManager.h"
#include "GameManager.h"
#include "RoomManager.h"
#include "MatchManager.h"
#include "DBManager.h"
#include "RedisManager.h"
#include "ConfigManager.h"


int main()
{

	gConfigManager->Init(current_path().string(), "Config\\config.json");

	try {
		PacketHandler::Init();

		Json::Value db = gConfigManager->GetJson("db");
		if (gDBManager->Init(db) == false)
		{
			throw std::exception("DB Init Failed");
		}

		//auto result = gDBManager->Exec<u_int>("com", "select unix_timestamp(now()) as dbtime;");
		//cout << *result.begin() << endl;

		Json::Value redis = gConfigManager->GetJson("redis");
		if (gRedisManager->Init(redis) == false)
		{
			throw std::exception("Redis Init Failed");
		}

		//gRedisManager->Exec("set test test");

	//	std::string tcpHost = "127.0.0.1";
	//	uint16		tcpPort = 7777;

	//	std::string grpcHost = "localhost";
	//	uint16		grpcPort = 5001;

	//	ServerEngineRef server = MakeShared<ServerEngine>(tcpHost, tcpPort, std::make_unique<IOCP>(), 5000, []() { return MakeShared<GameSession>(); }
	//		);
	//	//	
	//	if (server->Init() == false)
	//	{
	//		server = nullptr;

	//		throw std::exception("Server Init Failed");
	//	}
	//	std::cout << "TCP Server Start !" << std::endl;

	//	//if (gMatchManager->Init(grpcHost, grpcPort) == false)
	//	//{
	//	//		throw std::exception("Match Server Add Failed");
	//	//}
	//	//std::cout << "Regist At Match Server !" << std::endl;


	//	//gThreadManager->AddThread([]()
	//	//	{
	//	//		while (true)
	//	//		{
	//	//			MatchClient::CreateRoomJobBi();
	//	//		}
	//	//	});


		auto newRoom = gRoomManager->Add(1, 1, 10, 1);

	//	int threadCount = 6;

	//	gGameManager->DoTimer(250, &GameManager::Update);

	//	for (auto i = 0; i < threadCount; ++i)
	//	{
	//		gThreadManager->AddThread([&server]()
	//			{
	//				while (true)
	//				{
	//					TLS_LastTickCount = ::GetTickCount64() + 64;

	//					server->Run(10);

	//					// 예약된 일감 처리
	//					ThreadManager::DistributeReservedJobs();

	//					// 글로벌 큐
	//					ThreadManager::DoGlobalQueueWork();
	//				}
	//			});

	//}

	//	gThreadManager->Join();
	//	
	//		
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1; // 비정상 종료
	}	

	return 0;
}