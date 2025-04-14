#pragma once
#include "stdafx.h"

#include "SocketUtil.h"

class Overlapped;
class IIOCPObject
{
public:
    IIOCPObject() = default;
    virtual ~IIOCPObject() = default;

    HANDLE* GetHandle() const
    {
        return _handle.get();
    }

    void SetHandle(std::unique_ptr<HANDLE> handle)
    {
        _handle = std::move(handle);
    }

    virtual void Dispatch(std::shared_ptr<Overlapped> const iocpEvent, uint32_t const numOfBytes = 0) = 0;

private:
    std::unique_ptr<HANDLE> _handle;
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

	void IOWorkerFunc(uint32_t const timeout = INFINITE);

private:
	HANDLE _completionPort{ INVALID_HANDLE_VALUE };
};