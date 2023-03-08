#include "stdafx.h"
#include "View.h"
#include "Player.h"
#include "GameSession.h"

View::~View()
{
}

void View::Update(GameSessionRef session)
{
	PlayerRef owner = GetOwner();
	if (owner == nullptr)
	{
		mPrevActorSet.clear();
		return;
	}
	
	RoomRef room = owner->GetRoom(); 
	if (room == nullptr)
	{
		mPrevActorSet.clear();
		return;
	}

 	auto nowActorSet = room->GetNearActors(owner->mPos);

	std::set<ActorRef> newActorSet;
	std::set_difference(nowActorSet.begin(), nowActorSet.end(), mPrevActorSet.begin(), mPrevActorSet.end(), std::inserter(newActorSet, newActorSet.end()));

	std::set<ActorRef> toDeletedActorSet;
	std::set_difference(mPrevActorSet.begin(), mPrevActorSet.end(), nowActorSet.begin(), nowActorSet.end(), std::inserter(toDeletedActorSet, toDeletedActorSet.end()));

	for (auto actorRef : newActorSet)
	{
		Protocol::S_SPAWN spawnPkt;

		auto spawnActor = spawnPkt.add_actors();
		actorRef->SetActorInfo(spawnActor);

		session->Send(2, spawnPkt);
	}

	for (auto actorRef : toDeletedActorSet)
	{
		if (actorRef->mId == 0) continue; // 배경제외

		Protocol::S_DESPAWN despawnPkt;

		auto despawnActor = despawnPkt.add_actors();
		actorRef->SetActorInfo(despawnActor);

		session->Send(3, despawnPkt);
	}

	mPrevActorSet.clear();
	mPrevActorSet = nowActorSet;
}