#pragma once
#include <mysql/mysql.h>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <sstream>
#include "DBInterface.h"

#define U8TOW(x, y) MultiByteToWideChar(CP_UTF8, 0, (*p)[col].c_str(), strlen((*p)[col].c_str()), tmp.begin(), nLen)
#define WTOU8(x, y) CW2A(x, CP_UTF8)

#define START_MYSQL_RESULT(x) \
	MySqlResult *p = x; \
	MySqlResultColumnValue column; \
	int col = 0; 

#define BEGIN_MYSQL_RESULT(x) { \
	MySqlResult *p = x; \
	MySqlResultColumnValue column; \
	int col = 0; 

#define END_MYSQL_RESULT() };


struct MySqlResult : public std::vector<std::string>
{
	MySqlResult() { clear(); type.clear(); }
	~MySqlResult() { clear(); type.clear(); }
	std::vector<enum_field_types> type;
	void operator()(const char *p, enum_field_types t)
	{
		push_back(p);
		type.push_back(t);
	}
};

struct MySqlResultColumnValue
{
	void operator()(int &ivalue, MySqlResult *p, int col)
	{
		ivalue = std::stoi((*p)[col]);
	}

	void operator()(u_int &ivalue, MySqlResult *p, int col)
	{
		ivalue = std::stoi((*p)[col]);
	}

	void operator()(byte &bvalue, MySqlResult *p, int col)
	{
		bvalue = static_cast<byte>(std::stoi((*p)[col]));
	}

	void operator()(short &svalue, MySqlResult *p, int col)
	{
		svalue = std::stoi((*p)[col]);
	}

	void operator()(float &fvalue, MySqlResult *p, int col)
	{
		fvalue = std::stof((*p)[col]);
	}

	void operator()(u_short &svalue, MySqlResult *p, int col)
	{
		svalue = std::stoi((*p)[col]);
	}

	void operator()(int64_t &i64value, MySqlResult *p, int col)
	{
		i64value = std::stoll((*p)[col]);
	}

	void operator()(uint64_t &i64value, MySqlResult *p, int col)
	{
		i64value = std::stoll((*p)[col]);
	}

	void operator()(std::string &str, MySqlResult *p, int col, bool bU8 = true)
	{
		if (bU8)
		{
			std::wstring tmp;

			
			int nLen = MultiByteToWideChar(CP_UTF8, 0, (*p)[col].c_str(), strlen((*p)[col].c_str()), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, (*p)[col].c_str(), strlen((*p)[col].c_str()), tmp.data(), nLen);

			nLen = WideCharToMultiByte(CP_UTF8, 0, tmp.c_str(), lstrlenW(tmp.c_str()), NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, tmp.c_str(), lstrlenW(tmp.c_str()), str.data(), nLen, NULL, NULL);
		}
		else
		{
			str = (*p)[col];
		}
	}

	void operator()(char *pszStr, MySqlResult *p, int col, bool bU8 = true)
	{
		std::string str;
		operator()(str, p, col, bU8);

		size_t size = str.length() + 1;
		strcpy_s(pszStr, size, str.c_str());
	}

	void operator()(std::wstring &wstr, MySqlResult *p, int col, bool bU8 = true)
	{
		if (bU8)
		{
			int nLen = MultiByteToWideChar(CP_UTF8, 0, (*p)[col].c_str(), strlen((*p)[col].c_str()), NULL, NULL);
			MultiByteToWideChar(CP_UTF8, 0, (*p)[col].c_str(), strlen((*p)[col].c_str()), wstr.data(), nLen);
		}
		else
		{

			auto strMultibyte = (*p)[col].c_str();
			int nLen = MultiByteToWideChar(CP_ACP, 0, strMultibyte, strlen(strMultibyte), NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, strMultibyte, strlen(strMultibyte), wstr.data(), nLen);
		}
	}

	void operator()(wchar_t *pszStr, MySqlResult *p, int col, bool bU8 = true)
	{
		std::wstring str;
		operator()(str, p, col, bU8);
		size_t size = str.length() + 1;
		wcscpy_s(pszStr, size, str.c_str());
	}
};

class MysqlTrans
{
private:
	MYSQL *mpMysql;
	bool mCommit;
public:
	MysqlTrans(MYSQL *pMysql) : mCommit(false)
	{
		mpMysql = pMysql;
		mysql_autocommit(mpMysql, 0);
		//mysql_query(mpMysql, "BEGIN");
	}

	~MysqlTrans()
	{
		if (mCommit)
			mysql_commit(mpMysql);
		else
			mysql_rollback(mpMysql);

		mysql_autocommit(mpMysql, 1);
	}

	void commit()
	{
		mCommit = true;
	}
};

class Mysql : public DBInterface
{
public:
	Mysql(std::string_view user, std::string_view password, std::string_view host,int port, std::string_view dbName,int connectionLimit)
		: mUser(user),
		mPassword(password),
		mHost(host),
		mPort(port),
		mDBName(dbName),
		mConnectionLimit(connectionLimit)
	{
		mType = "mysql";
	}
	~Mysql()
	{
		if (mpMysql)
		{
			mysql_close(mpMysql);
			delete mpMysql;
			mpMysql = NULL;
		}
	}

public:
	virtual bool Init() override;

	virtual bool Exec(std::string query) override {
		if (sendQuery(query, &mAffectedRows, &mInsertedId) == false) return false;

	};

	virtual bool TransExec(list<std::string>&& querys)
	{
		MysqlTrans tran(mpMysql);
		for (auto& str : querys)
		{
			if (sendQuery(str, &mAffectedRows, &mInsertedId) == false)
				return false;
		}
		tran.commit();

		querys.clear();
		return true;
	};

	virtual bool Ping() override //MYSQL_OPT_RECONNECT 에의해 ping 호출이 리커넥트가 된다.
	{
		return mysql_ping(mpMysql);
	}

	std::list<std::shared_ptr<MySqlResult>>& GetRecords() { return mRecords; }

	const char* GetError()
	{
		return mError.c_str();
	}
	u_int GetErrorNo()
	{
		return mErrorNo;
	}

private:
	std::string mUser;
	std::string mPassword;
	std::string mHost;
	std::string mDBName;
	int mPort;
	int mConnectionLimit;

	bool mConnect;

	u_int mErrorNo = 0;
	std::string mError;


	int miFields = 0;
	std::list<std::shared_ptr<MySqlResult>> mRecords;
	std::list<std::string> mFields;
	my_ulonglong mAffectedRows = 0;
	my_ulonglong mInsertedId = 0;

	MYSQL *mpMysql;

private:
	virtual bool connect() override;

	void setError()
	{
		mErrorNo = mysql_errno(mpMysql);
		mError = mysql_error(mpMysql);
	}
	bool sendQuery(std::string query, my_ulonglong* affectedRows = nullptr, my_ulonglong* insertedId = nullptr);

};