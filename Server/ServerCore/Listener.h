#pragma once
#include "Session.h"
#include <functional>

class Listener
{
public:
	void Init(short port);
	void Start();
	void Stop();

private:
	SOCKET mListenSocket;
	char acceptBuff[64];
	
	void asyncAccept(std::function<std::shared_ptr<Session>()> sessionFactory);
};

static LPFN_ACCEPTEX FnAcceptEx = nullptr;