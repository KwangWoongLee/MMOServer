#pragma once
class User
{
public:
	User(GameSessionRef session, uint64_t aidx, string name, uint32_t roomId, PlayerRef player);
	virtual ~User();

	GameSessionRef GetSession() { return mSession.lock(); }

//private:
	uint64_t	mAidx;
	string	mName;
	uint32_t	mRoomId;

	bool PingCheck()
	{
		auto now = GetTickCount64();
		if (mPing > 0 && 100000 <  now - mPing)
		{
			return false;
		}

		return true;
	};

	uint64_t mPing = 0;

	weak_ptr<GameSession> mSession;
	PlayerRef mPlayer = nullptr;


};

