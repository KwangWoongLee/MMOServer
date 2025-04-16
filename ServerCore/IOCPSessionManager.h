#pragma once
#include "IOCPSession.h"
#include "ObjectPool.h"
#include "Singleton.h"

class IOCPSessionManager
{
public:
    using Singleton = Singleton<IOCPSessionManager>;

public:
    bool RegistListener(std::shared_ptr<IIOCPObject> const& iocpObject)
    {
        if (not iocpObject || INVALID_HANDLE_VALUE == iocpObject->GetHandle())
        {
            return false;
        }

        auto const listener = dynamic_pointer_cast<Listener>(iocpObject);
        if (not listener)
        {
            return false;
        }

        _listener = listener;

        return _iocp->RegistForCompletionPort(iocpObject);
    }

    std::shared_ptr<IOCPSession> CreateSession()
    {
        auto const iocpSession = ObjectPool<IOCPSession>::Singleton::Instance().Acquire();
        iocpSession->SetHandle(std::make_unique<HANDLE>(SocketUtil::Singleton::Instance().CreateSocket()));

        if (not _iocp->RegistForCompletionPort(iocpSession))
        {
            return nullptr;
        }
        
        return iocpSession;
    }

    void ReleaseSession(std::shared_ptr<IOCPSession> iocpSession)
    {
        SocketUtil::Singleton::Instance().CloseSocket(reinterpret_cast<SOCKET>(*iocpSession->GetHandle()));
        ObjectPool<IOCPSession>::Singleton::Instance().Release(iocpSession);
    }

private:
    IOCPSessionManager() = default;
    ~IOCPSessionManager() = default;

private:
    std::shared_ptr<IOCP> _iocp;
    std::shared_ptr<Listener> _listener;
};
