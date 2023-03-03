#pragma once
#include <json/json.h>

class DBInterface
{
public:
	virtual ~DBInterface() {};

	virtual bool Init() = 0;

	virtual bool Exec(std::string query) = 0;
	virtual bool TransExec(list<std::string>&& querys) = 0;
	virtual bool Ping() = 0;
	
	virtual bool connect() = 0;

	const std::string& GetType() const { return mType; }

	std::string mType;
};