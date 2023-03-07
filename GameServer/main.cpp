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
		cout << ("DB Ready ! ") << endl;

		//auto result = gDBManager->Exec<u_int>("com", "select unix_timestamp(now()) as dbtime;");
		//cout << *result.begin() << endl;

		Json::Value redis = gConfigManager->GetJson("redis");
		if (gRedisManager->Init(redis) == false)
		{
			throw std::exception("Redis Init Failed");
		}
		cout << ("Redis Ready ! ") << endl;

		//gRedisManager->Exec("set test1 test");

		//gRedisManager->DoAsync(&RedisManager::Exec, "set test2 test");

		string configName = "server";
#ifdef TEST
		configName = "test";
#endif // TEST


		std::string tcpHost = gConfigManager->Configs[configName]["host"].asString();
		uint16		tcpPort = gConfigManager->Configs[configName]["port"].asInt();
		uint32		maxSession = gConfigManager->Configs[configName]["maxSession"].asInt();
		float		viewSize = gConfigManager->Configs[configName]["viewSize"].asFloat();
		uint32		viewDelay = gConfigManager->Configs[configName]["viewDelay"].asInt();

		std::string grpcHost = gConfigManager->Configs["grpc"]["host"].asString();
		uint16		grpcPort = gConfigManager->Configs["grpc"]["port"].asInt();

		ServerEngineRef server = MakeShared<ServerEngine>(tcpHost, tcpPort, std::make_unique<IOCP>(), maxSession, []() { return MakeShared<GameSession>(); }
			);
		//	
		if (server->Init() == false)
		{
			server = nullptr;

			throw std::exception("Server Init Failed");
		}
		std::cout << "TCP Server Ready !" << std::endl;

		//if (gMatchManager->Init(grpcHost, grpcPort) == false)
		//{
		//		throw std::exception("Match Server Add Failed");
		//}


	//	//gThreadManager->AddThread([]()
	//	//	{
	//	//		while (true)
	//	//		{
	//	//			MatchClient::CreateRoomJobBi();
	//	//		}
	//	//	});
		std::cout << "Match Server Ready !" << std::endl;


		gRoomManager->Add(1, 1, 10, 1, viewSize, viewDelay);

		int threadCount = 10;

		gGameManager->DoTimer(250, &GameManager::Update);

		for (auto i = 0; i < threadCount; ++i)
		{
			gThreadManager->AddThread([&server]()
				{
					while (true)
					{
						TLS_LastTickCount = ::GetTickCount64() + 64;

						server->Run(10);

						ThreadManager::DistributeReservedJobs();

						ThreadManager::DoGlobalQueueWork();
					}
				});

	}

		gThreadManager->Join();
		
			
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1; // 비정상 종료
	}	

	return 0;
}