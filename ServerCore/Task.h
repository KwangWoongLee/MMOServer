#pragma once

class ITask
{
public:
	ITask() = default;
	virtual ~ITask() = default;

public:
	virtual void const Execute() = 0;
};