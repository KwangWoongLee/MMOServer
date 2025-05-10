#include "stdafx.h"
#include "ZoneServerApp.h"

int main()
{
    auto const app = std::make_unique<ZoneServerApp>();

    if (!app->Init())
    {
        std::cerr << "Init failed\n";
        return -1;
    }

    app->Run();
    return 0;
}
