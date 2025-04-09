#pragma once

class ITask
{
public:
	ITask() = default;
	virtual ~ITask() = default;

public:
	virtual void Excute() = 0;
};