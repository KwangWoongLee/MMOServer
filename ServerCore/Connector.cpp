#include "stdafx.h"
#include "Connector.h"
#include "IOCPSessionManager.h"
#include "SocketUtil.h"


bool Connector::Init(std::string const& ip, uint16_t const port)
{
    _ip = ip;
    _port = port;

    if (not _iocp->RegistForCompletionPort(shared_from_this()))
    {
        return false;
    }

    return true;
}

void Connector::AsyncConnect()
{
    auto* const ioEvent = ObjectPool<Overlapped>::Singleton::Instance().Acquire();
    ioEvent->Init();
    ioEvent->SetIOType(EIOType::CONNECT);

    auto const iocpSession = IOCPSessionManager::Singleton::Instance().CreateSession();
    ioEvent->SetIOCPObject(iocpSession);

    SOCKET const sock = reinterpret_cast<SOCKET>(iocpSession->GetHandle());

    SocketUtil::Singleton::Instance().SetReuseAddress(sock, true);
    SocketUtil::Singleton::Instance().SetLinger(sock, 0, 0);
    SocketUtil::Singleton::Instance().SetNoDelay(sock, true);

    iocpSession->SetHandle(reinterpret_cast<HANDLE>(sock));

    // 비동기 Connect 요청
    if (not FnConnectEx(sock, _ip.c_str(), _port, reinterpret_cast<LPOVERLAPPED>(&(*ioEvent))))
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            ObjectPool<Overlapped>::Singleton::Instance().Release(ioEvent);
        }
    }
}

void Connector::Dispatch(Overlapped const* ioEvent, uint32_t const numOfBytes)
{
    if (EIOType::CONNECT != ioEvent->GetIOType())
    {
        return;
    }

    auto const iocpObject = ioEvent->GetIOCPObject();
    auto const iocpSession = std::dynamic_pointer_cast<IOCPSession>(iocpObject);
    if (not iocpSession)
    {
        return;
    }

    SOCKET const socket = reinterpret_cast<SOCKET>(iocpSession->GetHandle());

    if (not SocketUtil::Singleton::Instance().SetUpdateConnectSocket(socket))
    {
        return;
    }

    if (not iocpSession->SetSockAddr())
    {
        return;
    }

    iocpSession->OnConnectCompleted(); // 연결 완료 처리
}