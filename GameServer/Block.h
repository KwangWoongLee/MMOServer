#pragma once
#include "GameTypes.h"
#include "Actor.h"
#include "Proto/protocol.pb.h"
#include "Room.h"

class Block : public Actor
{
public:
	Block(Protocol::BlockType blockType)
		:Actor(Protocol::ActorType::ACTOR_TYPE_BLOCK),
		mBlockType(blockType)
	{
		mHeightSize = 30;
		mWidthSize = 32;
	};
	virtual ~Block();

	virtual void Update() override {};
	virtual void SetActorInfo(Protocol::PActor* actor) override
	{
		Actor::SetActorInfo(actor);

		actor->set_blocktype(mBlockType);
	}
	virtual void SetDetailType(Protocol::PActor* actor) override
	{
		mBlockType = actor->blocktype();
	};

	Protocol::BlockType mBlockType = Protocol::BlockType::BLOCK_TYPE_NONE;
};

