#pragma once

class RoomManager
{
public:
	uint64_t Add(uint64_t hostAidx, uint32_t gameMapId, uint32_t maxMemberCount, uint32_t minMemberCount, float viewSize, uint32_t viewDelay);
	void Remove(uint64_t roomId);
	RoomRef Find(uint64_t roomId);

	void Update();

	static std::atomic<uint64_t> smRoomId;
private:
	USE_LOCK;

	std::map<uint64_t, RoomRef> mId2Room;
};

extern shared_ptr<RoomManager> gRoomManager;