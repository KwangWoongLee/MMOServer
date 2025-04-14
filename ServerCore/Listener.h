#pragma once
#include "IOCP.h"

class Listener
	: public IIOCPObject
	, public enable_shared_from_this<Listener>
{
public:
	virtual void Dispatch(Overlapped const* iocpEvent, uint32_t const numOfBytes = 0) override;

public:
	Listener() = default;
	virtual ~Listener();

public:
	bool Init();

private:
	void prepareAccepts();
	bool asyncAccept(AcceptEvent* acceptEvent);
};