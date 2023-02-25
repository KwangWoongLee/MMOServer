#pragma once
#include "MatchClient.h"

class MatchManager
{
public:
	MatchManager();
	~MatchManager();

	bool Init(std::string host, uint16 port);

	bool TestCreate();
	bool TestRemove();

private:
	bool mServerAdded = false;
	std::shared_ptr<MatchClient> mMatchClient = nullptr; 
};

extern std::shared_ptr<MatchManager> gMatchManager;