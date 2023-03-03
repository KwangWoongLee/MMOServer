#include "stdafx.h"
#include "Mysql.h"

bool Mysql::Init()
{
	mpMysql = new MYSQL;
	if(mysql_init(mpMysql) == nullptr) return false;

	my_bool	reconnect = 1;
	mysql_options(mpMysql, MYSQL_OPT_RECONNECT, &reconnect);

	return connect();
}

bool Mysql::connect()
{
	MYSQL* p = mysql_real_connect(mpMysql, mHost.c_str(), mUser.c_str(), mPassword.c_str(), mDBName.c_str(), mPort, NULL, 0);
	if (p)
	{
		mysql_query(mpMysql, "set names utf8");
		mConnect = true;
	}
	else
	{
		setError();
		mConnect = false;
	}


	return mConnect;
}

bool Mysql::sendQuery(std::string query, my_ulonglong* pAffected_rows, my_ulonglong* pInserted_id)
{
	int ret = mysql_query(mpMysql, query.c_str());
	if (ret != 0) // 0이 아니면 에러
	{
		setError();
		return false;
	}

	mRecords.clear();
	mFields.clear();

	miFields = 0;

	MYSQL_RES* pRes = mysql_use_result(mpMysql);
	MYSQL_ROW Row;

	if (pAffected_rows)
		*pAffected_rows = mysql_affected_rows(mpMysql);
	if (pInserted_id)
		*pInserted_id = mysql_insert_id(mpMysql);

	if (pRes == NULL)return true; // insert , update

	miFields = mysql_num_fields(pRes);
	bool bFirst = true;
	while (Row = mysql_fetch_row(pRes))
	{
		std::shared_ptr<MySqlResult> Record(new MySqlResult());

		if (bFirst)
		{
			for (int i = 0; i < miFields; i++)
				mFields.push_back(pRes->fields[i].name);
			bFirst = false;
		}

		for (int i = 0; i < miFields; i++)
		{
			(*Record)((Row[i]) ? Row[i] : "NULL", pRes->fields[i].type);
		}

		mRecords.push_back(Record);
	}

	mysql_free_result(pRes);

	return true;
}
