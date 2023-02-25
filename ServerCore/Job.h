#pragma once
using Callback = std::function<void()>;

class Job
{
public:
	Job(Callback&& callback) : mCallback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		mCallback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	void Execute()
	{
		mCallback();
	}

private:
	Callback mCallback;
};

