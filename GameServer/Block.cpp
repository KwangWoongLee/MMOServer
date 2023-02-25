#include "stdafx.h"
#include "Block.h"

Block::~Block()
{
}

void Block::Spawn(RoomRef room)
{
	Actor::Spawn(room);


	//mPlayerMap[actor->mId] = static_pointer_cast<Player>(actor);

	//
	//for (auto [id, playerRef] : mPlayerMap)
	//{
	//	auto actor = pkt.add_actors();

	//	actor->set_id(playerRef->mId);
	//	actor->set_name(playerRef->mName);
	//	actor->set_type(playerRef->mType);
	//	auto pos = actor->mutable_position();
	//	pos->set_x(playerRef->mPos.first);
	//	pos->set_y(playerRef->mPos.second);

	//	if (actor->type() == Protocol::ActorType::ACTOR_TYPE_PLAYER)
	//		actor->set_playertype(playerRef->mPlayerType);

	//}
}