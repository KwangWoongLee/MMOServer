#pragma once
#include "stdafx.h"

#include "SocketUtil.h"


enum class EIOType : uint8_t
{
	SEND,
	RECV,
	ACCEPT,
	CONNECT,
	DISCONNECT
};

class IIOCPObject;
class Overlapped : public OVERLAPPED
{
public:
	Overlapped(EIOType const type)
		: _ioType(type)
	{
	};

public:
	EIOType _ioType{};
	std::shared_ptr<IIOCPObject> _iocpObj;
};

class IIOCPObject
{
public:
	explicit IIOCPObject()
	{
		_handel = reinterpret_cast<HANDLE>(SocketUtil::Singleton::Instance().CreateSocket());
	}
	virtual ~IIOCPObject() = default;

public:
	HANDLE GetHandle() const { return _handel; }

	virtual void Dispatch(Overlapped const* iocpEvent, uint32_t const numOfBytes = 0) = 0;

private:
	HANDLE _handel;
};

class IOCP final
{
public:
	explicit IOCP()
	{
		_completionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

		if (_completionPort == nullptr)
		{
			std::abort();
		}
	}

	bool RegistForCompletionPort(std::shared_ptr<IIOCPObject> const& iocpObject);

	void Run(uint32_t timeout = INFINITE);
	void Stop();

	// IOWorker 쓰레드 함수
	void IOWorkerFunc(uint32_t const timeout = INFINITE);

private:
	HANDLE _completionPort{ INVALID_HANDLE_VALUE };
};