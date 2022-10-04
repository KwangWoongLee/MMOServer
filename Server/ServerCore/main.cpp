#include "stdafx.h"
#include "Server.h"


int main()
{
	//threadType = eThreadType::MAIN;
	try {
		Server server;
		server.Init(8080, 3);
		
		server.Run();
		
		while (true)
		{
			if (server.GetState() == Server::eState::STOP)
			{
				server.Stop();
				break;
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1; // 비정상 종료
	}	

	return 0;
}
