#pragma once
#include "GameTypes.h"
#include "Actor.h"
#include "Proto/protocol.pb.h"
//#include "Bomb.h"
#include "View.h"

class Player : public Actor
{
public:
	Player(){};
	Player(Protocol::PlayerType playerType)
		:Actor(Protocol::ActorType::ACTOR_TYPE_PLAYER),
		mPlayerType(playerType)
	{
		mHeightSize = 25;
		mWidthSize = 23;
		mSpeed = 10.f;
	};
	virtual ~Player();

	virtual void Update() override;
	virtual void SetActorInfo(Protocol::PActor* actor) override
	{
		Actor::SetActorInfo(actor);

		actor->set_playertype(mPlayerType);
	}
	virtual void SetDetailType(Protocol::PActor* actor) override
	{
		mPlayerType = actor->playertype();
	};



public:
	void SetRoom(RoomRef room) { mRoom = room; }
	RoomRef GetRoom() { return mRoom.lock(); }

	void SetUser(UserRef user) { mUser = user; }
	UserRef GetUser() { return mUser.lock(); }


	View& GetView() { return mView; }


public:
	Protocol::PlayerType    mPlayerType = Protocol::PlayerType::PLAYER_TYPE_NONE;
	weak_ptr<Room>			mRoom;
	weak_ptr<User>			mUser;
	View	mView;

	
	uint64_t				mKillCount = 0;
};