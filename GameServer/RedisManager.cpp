#include "stdafx.h"
#include "RedisManager.h"
#include <hiredis/hiredis.h>
#include <json/json.h>
#include <hiredis/read.h>

std::shared_ptr<RedisManager> gRedisManager = MakeShared<RedisManager>();


bool RedisManager::Init(Json::Value& redis)
{
	mHost = redis["host"].asString();
	mPort = redis["port"].asInt();
	mNo = redis["db"].asInt();

	if (connect() == false) {
		return false;
	}

	return true;
}


REDISRETPTR RedisManager::Exec(const char* cmd)
{
	REDISRETPTR uPtr;

	redisReply* pRet = command(cmd);

	if (pRet == nullptr)
	{
		if (GetErrorNo()) // reconnect
		{
			bool bRet = connect();
			if (bRet)
				pRet = command(cmd);
			else
				return nullptr;
		}
		else
		{
			return nullptr;
		}
	}

	if (pRet->type == REDIS_REPLY_NIL || pRet->type == REDIS_REPLY_ERROR)
	{
		replyFree();
		return nullptr;
	}

	uPtr.reset(new RedisReply);

	replyFlush(pRet, *uPtr);

	replyFree();

	return uPtr;
}


bool RedisManager::RemoveKey(const char* pKey)
{
	std::ostringstream oss;
	oss << "del " << pKey << "";
	REDISRETPTR uPtr = Exec(oss.str().c_str());
	if (uPtr == nullptr) return false;

	return true;
}

int RedisManager::GetErrorNo()
{
	if (mContext == nullptr)
		return -1;
	else
		return mContext->err;
}


void RedisManager::SetTTL(const char* pKey, u_int second)
{
	std::ostringstream oss;
	oss << "expire " << pKey << " " << second;
	Exec(oss.str().c_str());
}


REDISRETPTR RedisManager::Dump(const char* key)
{
	std::ostringstream oss;
	oss << "dump " << key;
	return Exec(oss.str().c_str());
}

bool RedisManager::Restore(const char* key, const char* buf, size_t len)
{
	redisReply* pRet = (redisReply*)redisCommand(mContext, "RESTORE %s 0 %b", key, buf, len);
	freeReplyObject(pRet);
	return (pRet == nullptr || pRet->type == REDIS_REPLY_ERROR) ? false : true;
}



bool RedisManager::connect()
{
	if (mContext)
		redisFree(mContext);

	mContext = redisConnect(mHost.c_str(), mPort);
	if (GetErrorNo())
	{
		if (mContext)
		{
			redisFree(mContext);
			mContext = NULL;
		}
		return false;
	}

	if (mAuth.length())
	{
		std::ostringstream oss;
		oss << "auth " << mAuth;
		redisReply* pRet = command(oss.str());
		if (pRet == NULL)return false;
		replyFree();
	}

	if (mNo)
	{
		std::ostringstream oss;
		oss << "select " << mNo;
		redisReply* pRet = command(oss.str());
		if (pRet == NULL)return false;
		replyFree();
	}
	return true;
}

void RedisManager::replyFlush(redisReply* pRet, RedisReply& ret)
{
	ret.type = pRet->type;
	ret.len = pRet->len;
	if (ret.type == REDIS_REPLY_STRING)
	{
		ret.str.append(pRet->str, pRet->len);
	}
	else if (ret.type == REDIS_REPLY_INTEGER)
	{
		ret.integer = pRet->integer;
	}
	else if (ret.type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < pRet->elements; i++)
		{
			RedisReply reply;
			replyFlush(pRet->element[i], reply);

			ret.element_list.push_back(reply);
		}
	}
}

redisReply* RedisManager::command(std::string&& cmd)
{
	if (mContext == nullptr) return nullptr;

	mRet = (redisReply*)redisCommand(mContext, cmd.c_str());
	if (mRet == nullptr || mRet->type == REDIS_REPLY_ERROR)
		replyFree();

	return mRet;
}

void RedisManager::replyFree()
{
	if (mRet)
	{
		freeReplyObject(mRet);
		mRet = nullptr;
	}
}
