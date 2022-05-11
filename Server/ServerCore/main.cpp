#include "stdafx.h"
#include "Server.h"


int main()
{
	//threadType = eThreadType::MAIN;
	try {
		Server server;
		server.Init();
		
		server.Run();
		//while (server.GetState() != Server::eState::STOP)
		while (true)
		{
			server.Stop();
			;
		}
	}
	catch (std::string& e)
	{
		std::cerr << e << std::endl;
		return -1; // 비정상 종료
	}	

	return 0;
}
