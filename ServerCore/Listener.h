#pragma once
#include "IOCP.h"

class Listener
	: public IIOCPObject
{
public:
	virtual void Dispatch(std::shared_ptr<Overlapped> iocpEvent, uint32_t const numOfBytes = 0) override;

public:
	Listener() = default;
	virtual ~Listener();

public:
	bool Init();

private:
	void prepareAccepts();
	void asyncAccept();
};