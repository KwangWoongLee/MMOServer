#pragma once
#include "Listener.h"

class Server
{
public:
	enum class eState
	{
		INIT,
		RUN,
		STOP,
	};

	void Init();
	void Run();
	void Stop();
	
	eState GetState() { return mState; }

private:
	Listener mListener;
	eState mState;
};

