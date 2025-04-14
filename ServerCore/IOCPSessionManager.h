#pragma once
#include "IOCPSession.h"
#include "ObjectPool.h"
#include "Singleton.h"

class IOCPSessionManager
{
public:
    using Singleton = Singleton<IOCPSessionManager>;

public:
    bool RegistListener(std::shared_ptr<IIOCPObject> const& listener)
    {
        if (not listener || INVALID_HANDLE_VALUE == listener->GetHandle())
        {
            return false;
        }

        return _iocp->RegistForCompletionPort(listener);
    }

    std::shared_ptr<IOCPSession> CreateSession()
    {
        auto const iocpSession = ObjectPool<IOCPSession>::Singleton::Instance().Acquire();
        if (not _iocp->RegistForCompletionPort(iocpSession))
        {
            return nullptr;
        }
        
        return iocpSession;
    }

private:
    IOCPSessionManager() = default;
    ~IOCPSessionManager() = default;

private:
    std::shared_ptr<IOCP> _iocp;
};
