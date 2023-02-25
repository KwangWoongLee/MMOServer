#pragma once
class User
{
public:
	User(GameSessionRef session, uint64 aidx, string name, uint32 roomId, PlayerRef player);
	virtual ~User();

	GameSessionRef GetSession() { return mSession.lock(); }

//private:
	uint64	mAidx;
	string	mName;
	uint32	mRoomId;

	bool PingCheck()
	{
		auto now = GetTickCount64();
		if (mPing > 0 && 100000 <  now - mPing)
		{
			return false;
		}

		return true;
	};

	uint64 mPing = 0;

	weak_ptr<GameSession> mSession;
	PlayerRef mPlayer = nullptr;


};

