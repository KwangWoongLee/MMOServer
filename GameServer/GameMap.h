#pragma once
class GameMap
{
public:
	GameMap() = delete;
	GameMap(uint8 id);

	bool Init(RoomRef room);
	bool ApplyMove(ActorRef actor, Position dest);

	Vector<short> mMapRange; // 0 : minX , 1 : maxX, 2 : minY, 3 : maxY
private:
	uint8 mId{};
	weak_ptr<Room> mRoom;
//

	Vector<Vector<short>> mMapCollision;
	Vector<Vector<short>> mMap;

	bool canGo(ActorRef actor, Position dest);
	
	bool isInRange(Position dest);

	bool loadData(std::string_view mapFileName = "./Map.csv", std::string_view collisionFileName = "./MapCollision.csv");
	bool spawnMapActor();
};

