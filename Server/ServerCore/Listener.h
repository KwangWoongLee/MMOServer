#pragma once
class Listener
{
public:
	void Init(short port);

private:
	SOCKET mListenSocket;

};

