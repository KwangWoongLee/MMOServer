#pragma once
#include "GameTypes.h"
#include "Room.h"

class Actor : public enable_shared_from_this<Actor>
{
public:

	Actor() {};
	Actor(Protocol::ActorType type);
	virtual ~Actor() {
	};

	virtual void Update() = 0;

	virtual void SetActorInfo(Protocol::PActor* actor) {
		actor->set_id(mId);
		actor->set_type(mType);

		auto pos = actor->mutable_position();
		pos->set_x(mPos.x);
		pos->set_y(mPos.y);
	};
	virtual void SetDetailType(Protocol::PActor* actor) {};


	void SetId(uint64_t id) { mId = id; }
	void SetPosition(Position pos, bool randPos = false);

	float	GetSpeed() const { return mSpeed; }
	void	SetSpeed(float speed) { mSpeed = speed; }

	ActorRef GetActorRef() { return shared_from_this(); };


public:
	uint64_t					mId{};
	string					mName{};
	Position				mPos{};
	State					mState = State::LIVE;
	weak_ptr<Room>			mRoom;
	float	mSpeed;

	uint16_t					mHeightSize = 0;
	uint16_t					mWidthSize = 0;

	Protocol::ActorType		mType = Protocol::ActorType::ACTOR_TYPE_NONE;

	bool operator ==(const ActorRef& other) {
		return other->mId == mId;
	}

	bool operator <(ActorRef & other) {
		return mId < other->mId ;
	}
};


class BG : public Actor
{
public:
	BG()
		:Actor(Protocol::ActorType::ACTOR_TYPE_BG)
	{}
	virtual ~BG() {};

	virtual void Update() {};
};

