#include "stdafx.h"
#include "SessionManager.h"

void SessionManager::Add(const std::shared_ptr<Session>&& session)
{
	//TODO : Lock �߰�
	session->SetKey(++mSessionCount);

	//TODO : insert ���ۿ��� Ȯ���ϰ� insert�� �� ��, []�� �� �� ����
	mSessionMap.insert({ mSessionCount, session });
}
