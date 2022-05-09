#include "stdafx.h"
#include "Listener.h"

int main()
{
	try {
		Listener listener;
		listener.Init(7700);
	}
	catch (std::string& e)
	{
		std::cerr << e << std::endl;
		return -1; // 비정상 종료
	}	

	while (true)
	{
		;
	}
}
