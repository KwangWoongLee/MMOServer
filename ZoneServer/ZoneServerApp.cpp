#include "stdafx.h"
#include "ZoneServerApp.h"

bool ZoneServerApp::Init()
{
    _iocp = std::make_shared<IOCP>();
    _clientSessionManager = std::make_unique<ZoneClientSessionManager>();
    _worldSessionManager = std::make_unique<ZoneWorldSessionManager>();

    return true;
}

void ZoneServerApp::Run()
{
    _iocp->Run();
}

void ZoneServerApp::Stop()
{
    _iocp->Stop();
}