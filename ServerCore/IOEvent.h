#pragma once
#include "stdafx.h"

enum class EIOType : uint8_t
{
    SEND,
    RECV,
    ACCEPT,
    CONNECT,
    DISCONNECT
};

class IIOCPObject;
class IOCPSession;

class Overlapped : 
    public OVERLAPPED
{
public:
    Overlapped() = default;
    explicit Overlapped(EIOType const type)
        : _ioType(type)
    {
        Init();
    }

public:
    void Init()
    {
        ZeroMemory(static_cast<OVERLAPPED*>(this), sizeof(OVERLAPPED));
        _iocpObj = nullptr;
    }

    EIOType GetIOType() const
    {
        return _ioType;
    }

    std::shared_ptr<IIOCPObject> GetIOCPObject() const
    {
        return _iocpObj;
    }

    void SetIOCPObject(std::shared_ptr<IIOCPObject> const iocpObj)
    {
        _iocpObj = iocpObj;
    }

private:
    EIOType _ioType{};

    std::shared_ptr<IIOCPObject> _iocpObj;
};

class AcceptEvent
    : public Overlapped
{
public:
    AcceptEvent() 
        : Overlapped(EIOType::ACCEPT)
    {
    };
};

class ConnectEvent
    : public Overlapped
{
public:
    ConnectEvent()
        : Overlapped(EIOType::CONNECT)
    {
    };
};


class DisconnectEvent
    : public Overlapped
{
public:
    DisconnectEvent()
        : Overlapped(EIOType::DISCONNECT)
    {
    };
};

class RecvEvent
    : public Overlapped
{
public:
    RecvEvent()
        : Overlapped(EIOType::RECV)
    {
    };
};

class SendEvent
    : public Overlapped
{
public:
    SendEvent()
        : Overlapped(EIOType::SEND)
    {
    };
};
