#pragma once

#include <hiredis/hiredis.h>

//#include "gvalue.h"
//#include "ICommon.h"
//
///*----- hiredis reply return type ---
//REDIS_REPLY_STRING 1
//REDIS_REPLY_ARRAY 2
//REDIS_REPLY_INTEGER 3
//REDIS_REPLY_NIL 4
//REDIS_REPLY_STATUS 5
//REDIS_REPLY_ERROR 6 */
//
//struct RedisReply
//{
//	int type = 0;
//	__int64 integer = -1;
//	std::string str;
//	int len = 0;
//	std::list<RedisReply> element_list;
//};
//
//typedef std::unique_ptr<RedisReply> REDISRETPTR;
//
class RedisManager
{
//private:
//	ILock mCS;
//	redisContext* m_pctx = NULL;
//	redisReply* m_pret = NULL;
//	std::string m_host, m_auth;
//	int m_port = 0, m_db_no = 0;
//public:
//	RedisManager() {}
//	~RedisManager()
//	{
//		if (m_pret)
//			freeReplyObject(m_pret);
//		if (m_pctx)
//			redisFree(m_pctx);
//	}
//
//	redisContext* get_ctx()
//	{
//		return m_pctx;
//	}
//
//	bool connect(const char* json_str)
//	{
//		WSADATA WSAData;
//		if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
//			return false;
//
//		Json::Value root;
//		bool bRead = gv::str_to_jsonval(json_str, root);
//		if (!bRead)return false;
//
//		m_auth = "";
//		m_db_no = 0;
//
//		m_host = root["host"].asCString();;
//		if (root["auth"].isNull() == false)
//			m_auth = root["auth"].asCString();;
//
//		m_port = root["port"].asInt();
//		if (root["db"].isNull() == false)
//			m_db_no = root["db"].asInt();
//
//		return connect();
//	}
//
//	void ping(std::wstring& msg)
//	{
//		auto ret = exec("ping");
//	}
//
//	REDISRETPTR dump(const char* key)
//	{
//		std::ostringstream oss;
//		oss << "dump " << key;
//		return exec(oss.str().c_str());
//	}
//
//	bool restore(const char* key, const char* buf, size_t len)
//	{
//		ISyncObject sync(mCS);
//		redisReply* pRet = (redisReply*)redisCommand(get_ctx(), "RESTORE %s 0 %b", key, buf, len);
//		freeReplyObject(pRet);
//		return (pRet == NULL || pRet->type == REDIS_REPLY_ERROR) ? false : true;
//	}
//
//	REDISRETPTR exec(const wchar_t* pCmd)
//	{
//		return exec(WTOU8(pCmd));
//	}
//
//	REDISRETPTR exec(const char* pCmd)
//	{
//		ISyncObject sync(mCS);
//
//		REDISRETPTR uPtr;
//
//		redisReply* pRet = Command(pCmd);
//
//		if (pRet == NULL)
//		{
//			if (GetErrorNo()) // reconnect
//			{
//				bool bRet = connect();
//				if (bRet)
//					pRet = Command(pCmd);
//				else
//					return nullptr;
//			}
//			else
//			{
//				return nullptr;
//			}
//		}
//
//		if (pRet->type == REDIS_REPLY_NIL || pRet->type == REDIS_REPLY_ERROR)
//		{
//			ReplyFree();
//			return nullptr;
//		}
//
//		uPtr.reset(new IRedisReply);
//
//		replay_flush(pRet, *uPtr);
//
//		ReplyFree();
//
//		return uPtr;
//	}
//
//	void SetTTL(const char* pKey, u_int second)
//	{
//		std::ostringstream oss;
//		oss << "expire " << pKey << " " << second;
//		exec(oss.str().c_str());
//	}
//
//	bool RemoveKey(const char* pKey)
//	{
//		std::ostringstream oss;
//		oss << "del " << pKey << "";
//		REDISRETPTR uPtr = exec(oss.str().c_str());
//		if (uPtr == nullptr) return false;
//
//		return true;
//	}
//
//	int GetErrorNo()
//	{
//		if (m_pctx == NULL)
//			return -1;
//		else
//			return m_pctx->err;
//	}
//
//private:
//	bool connect()
//	{
//		ISyncObject sync(mCS);
//
//		if (m_pctx)
//			redisFree(m_pctx);
//
//		m_pctx = redisConnect(m_host.c_str(), m_port);
//		if (GetErrorNo())
//		{
//			if (m_pctx)
//			{
//				redisFree(m_pctx);
//				m_pctx = NULL;
//			}
//			TRACE("err connect redis retry !!!\n");
//			return false;
//		}
//
//		if (m_auth.length())
//		{
//			std::ostringstream oss;
//			oss << "auth " << m_auth;
//			redisReply* pRet = Command(oss.str());
//			if (pRet == NULL)return false;
//			ReplyFree();
//		}
//
//		if (m_db_no)
//		{
//			std::ostringstream oss;
//			oss << "select " << m_db_no;
//			redisReply* pRet = Command(oss.str());
//			if (pRet == NULL)return false;
//			ReplyFree();
//		}
//		return true;
//	}
//
//	void replay_flush(redisReply* pRet, IRedisReply& ret)
//	{
//		ret.type = pRet->type;
//		ret.len = pRet->len;
//		if (ret.type == REDIS_REPLY_STRING)
//		{
//			ret.str.append(pRet->str, pRet->len);
//		}
//		else if (ret.type == REDIS_REPLY_INTEGER)
//		{
//			ret.integer = pRet->integer;
//		}
//		else if (ret.type == REDIS_REPLY_ARRAY)
//		{
//			for (size_t i = 0; i < pRet->elements; i++)
//			{
//				IRedisReply reply;
//				replay_flush(pRet->element[i], reply);
//
//				ret.element_list.push_back(reply);
//			}
//		}
//	}
//
//	redisReply* Command(const wchar_t* pCmd)
//	{
//		std::string cmd = WTOU8(pCmd);
//		return Command(cmd);
//	}
//
//	redisReply* Command(const char* pCmd)
//	{
//		std::string cmd = pCmd;
//		return Command(cmd);
//	}
//
//	redisReply* Command(std::string& cmd)
//	{
//		TRACE("redis cmd : %s\n", cmd.c_str());
//		if (m_pctx == NULL) return NULL;
//
//		m_pret = (redisReply*)redisCommand(m_pctx, cmd.c_str());
//		if (m_pret == NULL || m_pret->type == REDIS_REPLY_ERROR)
//			ReplyFree();
//
//		return m_pret;
//	}
//
//	void ReplyFree()
//	{
//		if (m_pret)
//		{
//			freeReplyObject(m_pret);
//			m_pret = NULL;
//		}
//	}
};
