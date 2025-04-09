#pragma once
#include "GameTypes.h"
#include "Actor.h"
#include "Proto/protocol.pb.h"
#include "View.h"

class Bomb : public Actor
{
public:
	Bomb(PlayerRef owner)
		:Actor(Protocol::ActorType::ACTOR_TYPE_BOMB),
		mOwner(owner)
	{
		mHeightSize = 25;
		mWidthSize = 23;
	};
	virtual ~Bomb()
	{
		cout << "~Bomb()" << endl;
	};

	virtual void Update() override;

	PlayerRef GetOwner() { return mOwner.lock(); }
	bool IsInRange(ActorRef actor);

	Set<uint64_t> OnUserIds;

private:
	std::weak_ptr<Player> mOwner;
	uint8_t mPower = 1;
};