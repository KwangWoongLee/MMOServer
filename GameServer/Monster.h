#pragma once
#include "GameTypes.h"
#include "Actor.h"
#include "Proto/protocol.pb.h"

class Monster : public Actor
{
public:
	Monster() {};
	Monster(Protocol::MonsterType MonsterType)
		:Actor(Protocol::ActorType::ACTOR_TYPE_MONSTER),
		mMonsterType(MonsterType)
	{
		mHeightSize = 25;
		mWidthSize = 23;
		mSpeed = 5.f;
	};
	virtual ~Monster();

	virtual void Update() override;
	virtual void SetActorInfo(Protocol::PActor* actor) override
	{
		Actor::SetActorInfo(actor);

		actor->set_monstertype(mMonsterType);
	}
	virtual void SetDetailType(Protocol::PActor* actor) override
	{
		mMonsterType = actor->monstertype();
	};



public:
	void SetRoom(RoomRef room) { mRoom = room; }
	RoomRef GetRoom() { return mRoom.lock(); }

public:
	Protocol::MonsterType    mMonsterType = Protocol::MonsterType::MONSTER_TYPE_NONE;
	weak_ptr<Room>			mRoom;
};