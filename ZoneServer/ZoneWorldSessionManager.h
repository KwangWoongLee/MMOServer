#pragma once
#include "stdafx.h"
#include "ZoneWorldSession.h"

class ZoneWorldSessionManager final : public BaseSessionManager<ZoneWorldSession>
{
public:
    ZoneWorldSessionManager() = default;
    ~ZoneWorldSessionManager() = default;
};
