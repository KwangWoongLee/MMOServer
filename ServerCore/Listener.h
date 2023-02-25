#pragma once
#include "IOCP.h"

// 


class Listener : public IOCPObject
{
public:
	//�߻�Ŭ���� IOCPObject ���������Լ� �������̵�
	virtual HANDLE GetHandle() override { return reinterpret_cast<HANDLE>(mListenSocket); }
	virtual void Dispatch(class Overlapped* iocpEvent, uint32 numOfBytes = 0) override;

public:
	Listener() = delete;

	Listener(ServerEngineRef server);
	virtual ~Listener();

public:
	bool Init();

private:
	SOCKET mListenSocket = INVALID_SOCKET;
	ServerEngineRef mServer = nullptr;
	std::vector<AcceptEvent*> mAcceptEvents;

	void prepareAccepts();
	void asyncAccept(AcceptEvent* acceptEvent);

	bool listen(int32 backlog = SOMAXCONN);
	bool bind();	

};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}