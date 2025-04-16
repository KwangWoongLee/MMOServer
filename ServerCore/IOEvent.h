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

class Overlapped 
    : public OVERLAPPED
    , public enable_shared_from_this<Overlapped>
{
public:
    Overlapped() = default;
    virtual ~Overlapped()
    {
        ObjectPool<Overlapped>::Singleton::Instance().Release(shared_from_this());
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

    void SetIOType(EIOType const ioType)
    {
        _ioType = ioType;
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