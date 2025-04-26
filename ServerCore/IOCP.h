#pragma once
#include "stdafx.h"

#include "SocketUtil.h"

class Overlapped;

class IIOCPObject
	: public enable_shared_from_this<IIOCPObject>
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

    virtual void Dispatch(Overlapped const* iocpEvent, uint32_t const numOfBytes = 0) = 0;

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

	bool RegistForCompletionPort(std::shared_ptr<IIOCPObject> const& iocpObject) const;

	void Run(uint32_t timeout = INFINITE) const;
	void Stop() const;

	void IOWorkerFunc(uint32_t const timeout = INFINITE) const;

public:
	static ULONG_PTR constexpr SHUTDOWN_KEY{ static_cast<ULONG_PTR>(-1) };

private:
	HANDLE _completionPort{ INVALID_HANDLE_VALUE };
};