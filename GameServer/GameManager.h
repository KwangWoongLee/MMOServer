#pragma once
class GameManager : public JobQueue
{
public:
	GameManager();
	~GameManager();

	void Update();
};

extern std::shared_ptr<GameManager> gGameManager;