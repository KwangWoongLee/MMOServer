#include "stdafx.h"
#include "Server.h"

#include "GRPC.h"


int main()
{
	//threadType = eThreadType::MAIN;
	//try {
	//	int threadCount = 1;

	//	ServerEngineRef server = std::make_shared<ServerEngine>("127.0.0.1", 9999, std::make_unique<IOCP>(), 1000, threadCount,
	//		std::make_shared<Session>()
	//		);
	//	
	//	if (server->Init())
	//	{
	//		server->Run();
	//	}
	//	else
	//	{
	//		server = nullptr;

	//		return 1;
	//	}
	//	
	//}
	//catch (std::exception& e)
	//{
	//	std::cerr << e.what() << std::endl;
	//	return -1; // 비정상 종료
	//}	

	//return 0;

	std::string server_address{ "localhost:5001" };
	GRPC grpc{ grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()) };
	auto response_user = grpc.AddServer();
}
