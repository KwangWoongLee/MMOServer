#pragma once
class Zone
{
public:
	Zone() = delete;
	Zone(short x, short y, RoomRef room);
	virtual ~Zone();

	bool Init();

	void AddUser(UserRef user);
	void RemoveUser(UserRef user);

	void AddActor(ActorRef actor);
	void RemoveActor(ActorRef actor);


	Vector<UserRef>& GetUsers() { return mUsers; }
	Vector<ActorRef>& GetActors() { return mActors; }

private:
	weak_ptr<Room> mRoom;
	Vector<UserRef> mUsers;
	Vector<ActorRef> mActors;

	short mMapX;
	short mMapY;
};

