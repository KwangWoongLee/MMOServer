#pragma once

class RoomManager
{
public:
	uint64 Add(uint64 hostAidx, uint32 gameMapId, uint32 maxMemberCount, uint32 minMemberCount, float viewSize, uint32 viewDelay);
	void Remove(uint64 roomId);
	RoomRef Find(uint64 roomId);

	void Update();

	static std::atomic<uint64> smRoomId;
private:
	USE_LOCK;

	std::map<uint64, RoomRef> mId2Room;
};

extern shared_ptr<RoomManager> gRoomManager;