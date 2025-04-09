#pragma once
#include <hiredis/hiredis.h>
#include <json/json.h>
#include <hiredis/read.h>


/*----- hiredis reply return type ---
REDIS_REPLY_STRING 1
REDIS_REPLY_ARRAY 2
REDIS_REPLY_INTEGER 3
REDIS_REPLY_NIL 4
REDIS_REPLY_STATUS 5
REDIS_REPLY_ERROR 6 */

struct RedisReply
{
	int type = 0;
	__int64_t integer = -1;
	std::string str;
	int len = 0;
	std::list<RedisReply> element_list;
};

typedef std::unique_ptr<RedisReply> REDISRETPTR;

class RedisManager : public JobQueue
{
public:
	RedisManager() {}
	~RedisManager()
	{
		if (mRet)
			freeReplyObject(mRet);
		if (mContext)
			redisFree(mContext);
	}

	bool Init(Json::Value& redis);
	

	REDISRETPTR Exec(std::string query);
	bool RemoveKey(const char* pKey);


	REDISRETPTR Dump(const char* key);
	void SetTTL(const char* pKey, u_int second);
	bool Restore(const char* key, const char* buf, size_t len);
	int GetErrorNo();


	bool Ping()
	{
		auto ret = Exec("ping");
		if (ret != nullptr)
			return true;

		return false;
	}


private:
	bool connect();
	void replyFlush(redisReply* pRet, RedisReply& ret);
	void replyFree();

	redisReply* command(std::string&& cmd);
	REDISRETPTR exec(const char* cmd);



private:
	redisContext* mContext = NULL;
	redisReply* mRet = NULL;
	std::string mHost, mAuth;
	int mPort = 0, mNo = 0;
};

extern std::shared_ptr<RedisManager> gRedisManager;