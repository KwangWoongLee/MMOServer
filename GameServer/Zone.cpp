#include "stdafx.h"
#include "Zone.h"
#include  <algorithm>

Zone::Zone(short x, short y, RoomRef room)
    : mMapX(x),
    mMapY(y),
    mRoom(room)
{
}

Zone::~Zone()
{
    mUsers.clear();
}

bool Zone::Init()
{
    return false;
}

void Zone::AddUser(UserRef user)
{
    mUsers.push_back(user);
}

void Zone::RemoveUser(UserRef user)
{   
    mUsers.erase(std::remove(mUsers.begin(), mUsers.end(), user));
}

void Zone::AddActor(ActorRef actor)
{
    mActors.push_back(actor);
}

void Zone::RemoveActor(ActorRef actor)
{
    mActors.erase(std::remove(mActors.begin(), mActors.end(), actor));
}
