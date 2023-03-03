#include "stdafx.h"
#include "DBManager.h"
#include <json/json.h>
#include "DBInterface.h"
#include "Mysql.h"

std::shared_ptr<DBManager> gDBManager = MakeShared<DBManager>();


bool DBManager::Init(Json::Value& dbs)
{
	for (auto db : dbs)
	{
		
		string customDBName = db["customName"].asString();
		string dbType = db["type"].asString();
		string user = db["user"].asString();
		string password = db["password"].asString();
		string host = db["host"].asString();
		int port = db["port"].asInt();
		string dbName = db["database"].asString();
		int connectionLimit = db["connectionLimit"].asInt();

		DBInterfaceRef dbRef = nullptr;


		// 종류가 몇개 안되므로 그냥 if문으로 판별
		if (dbType == "mysql")
		{
			dbRef = MakeShared<class Mysql>(user, password, host, port, dbName, connectionLimit);
		}
		//else if ()
		//{
		//}

		if (dbRef == nullptr)
			return false;

		if(dbRef->Init() == false)
			return false;

		add(customDBName, dbRef);
	}

	return true;
}


