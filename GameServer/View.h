#pragma once

class View
{
public:
	View()	{};
	~View();

	void Update(GameSessionRef session);
	void Destroy()
	{
		mPrevActorSet.clear();
	};

	void		SetOwner(PlayerRef owner) { mOwner = owner; }
	PlayerRef	GetOwner() { return mOwner.lock(); }

private:
	weak_ptr<Player> mOwner;
	std::set<ActorRef> mPrevActorSet;

};

