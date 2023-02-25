#pragma once

//�߻� Ŭ����
using SessionFactory = std::function<SessionRef()>;

class Engine : public EnableShared<Engine>
{
public:


public:
	//�����Լ� ����
	Engine(IOCPRef iocp, uint16 maxSessionCount, SessionFactory sessionFactory);
	virtual ~Engine();

	virtual bool Init();
	virtual void Run(uint32 timeout = INFINITE);
	virtual void Stop();

	SessionRef CreateSession();
	void		DisConnectSession(SessionRef session);
	bool RegistForCompletionPort(IOCPObjectRef iocpObject);

public:
	uint16 GetMaxSessionCount() const { return mMaxSessionCount; }


protected:
	USE_LOCK;

	IOCPRef					mIOCP = nullptr;
	uint16					mMaxSessionCount = 0;
	SessionFactory			mSessionFactory;
	std::vector<SessionRef>	mSessions;

};



