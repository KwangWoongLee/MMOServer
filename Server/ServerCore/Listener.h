#pragma once
#include "stdafx.h"

class Listener
{
public:
	void Init(short port);
	void Start();
	void Stop();

private:
	SOCKET mListenSocket;
	char acceptBuff[64];
	
	void asyncAccept(auto sessionFactory);
};
