#pragma once
class Position
{
public:
	Position() = default;
	Position(float x, float y) 
		:x(x),
		y(y)
	{}

	float x;
	float y;

	bool operator==(Position& other)
	{
		return this->x == other.x && this->y == other.y;
	}

};

