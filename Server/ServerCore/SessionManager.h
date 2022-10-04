#pragma once
#include "stdafx.h"

class SessionManager : public Singleton<SessionManager>
{
public:
	SessionManager() {};
	virtual ~SessionManager() {};

	void Add(const std::shared_ptr<Session>&& session);


private:
	std::unordered_map<size_t, std::shared_ptr<Session>> mSessionMap;
	size_t mSessionCount = 0;
};

