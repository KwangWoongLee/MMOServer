#pragma once
#include "stdafx.h"
#include "ZoneClientSession.h"

class ZoneClientSessionManager final : public BaseSessionManager<ZoneClientSession>
{
public:
    ZoneClientSessionManager() = default;
    ~ZoneClientSessionManager() = default;
};
