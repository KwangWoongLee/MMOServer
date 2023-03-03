#pragma once
#include <json/json.h>
#include "Mysql.h"

using DBInterfaceRef = shared_ptr<class DBInterface>;

class DBManager : public JobQueue
{
public:
	DBManager() {}
	~DBManager() {}

	bool Init(Json::Value& db);

	//void Exec(std::string customDBName, std::string query);

	template<typename Value>
	std::list<Value> Exec(std::string customDBName, std::string query)
	{
		std::list<Value> ret;

		auto db = mDBs[customDBName];
		auto dbType = db->GetType();

		if (db->Exec(query) == false)
		{
			// TODO : 로그남기기
			return ret;
		}



		// 종류가 몇개 안되므로 그냥 if문으로 판별
		if (dbType == string("mysql"))
		{
			auto mysql = static_pointer_cast<Mysql>(db);

			Value v;
			for (const auto& iter : mysql->GetRecords())
			{
				BEGIN_MYSQL_RESULT(iter.get());

				column(OUT v, iter.get(), 0);
				ret.push_back(v);

				END_MYSQL_RESULT();
			}

			return ret;
		}

	}
	

private:
	void add(std::string DB, DBInterfaceRef dbRef)
	{
		if(mDBs.find(DB) == mDBs.end())
			mDBs[DB] = dbRef;
	}


private:
	Map<std::string, DBInterfaceRef> mDBs;
};

extern std::shared_ptr<DBManager> gDBManager;