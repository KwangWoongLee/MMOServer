#pragma once

//추상 클래스
using SessionFactory = std::function<void()>;

class Engine : public EnableShared<Engine>
{
public:


public:
	//가상함수 선언
	Engine(std::string_view ip, uint16 port, std::unique_ptr<IOCP>&& iocp, uint16 maxSessionCount, uint8 threadCount, SessionFactory sessionFactory);
	virtual ~Engine();

	virtual bool Init();
	virtual void Run();
	virtual void Stop();

	bool RegistForCompletionPort(IOCPObjectRef iocpObject);

public:
	uint16 GetMaxSessionCount() const { return mMaxSessionCount; }

protected:
	std::unique_ptr<IOCP>	mIOCP = nullptr;
	uint16					mMaxSessionCount = 0;
	uint8					mThreadCount = 0;
	SessionFactory			mSessionFactory;

};



