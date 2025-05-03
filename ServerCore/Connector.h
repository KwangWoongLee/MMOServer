#pragma once
#include "IOCP.h"

class Connector final
    : public IIOCPObject
{
public:
    explicit Connector(std::shared_ptr<IOCP> const& iocp)
	    :_iocp(iocp)
	{}

    void Dispatch(Overlapped const* iocpEvent, uint32_t const numOfBytes = 0) override;

public:
    bool Init(std::string const& ip, uint16_t const port);
    void AsyncConnect();

private:
    std::shared_ptr<IOCP> const _iocp;

    std::string _ip;
    uint16_t _port{};
};