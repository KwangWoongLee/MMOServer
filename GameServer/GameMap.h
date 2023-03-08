#pragma once
class GameMap
{
public:
	GameMap() = delete;
	GameMap(uint8 id);

	bool Init(RoomRef room);
	bool SpawnMapActor();

	bool ApplyMove(ActorRef actor, Position dest);
	void Occupy(short x, short y);
	void Away(short x, short y);
	bool IsOccupied(short x, short y);
	Position SearchMapPosition(Position pos);
	std::pair<short, short> SearchMapIndex(Position pos);
	short GetMaxX() const { return mMapRange[1]; }
	short GetMaxY() const { return mMapRange[3]; }

	
	
private:
	uint8 mId{};
	weak_ptr<Room> mRoom;


	Vector<Vector<short>> mMapCollision;
	Vector<Vector<short>> mMap;
	Vector<short> mMapRange; // 0 : minX , 1 : maxX, 2 : minY, 3 : maxY

	bool canGo(ActorRef actor, Position dest);
	
	bool isInRange(Position dest);

	bool loadData();
	
};

