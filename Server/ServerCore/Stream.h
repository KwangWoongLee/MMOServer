#pragma once
#include "stdafx.h"

using PACKET_SIZE = size_t;

class Stream
{
public:
	Stream();


	const char* GetBuffer() { return mBuffer; }
	PACKET_SIZE	GetLength() { return mHead; }


protected:
	void	ReallocBuffer(size_t size);

	char* mBuffer;
	PACKET_SIZE	mHead;
	size_t	mCapacity;


};



class OutputStream : public Stream
{
public:
	OutputStream();



	template< typename T >
	void operator << (const T& t)
	{
		this->write(t);
	}

	template<>
	void operator << (const std::vector< int >& intVector)
	{
		size_t size = intVector.size();
		this->write(size);
		this->write(intVector.data(), size * sizeof(int));
	}

	template<>
	void operator << (const std::string& string)
	{
		size_t size = string.size();

		this->write(size);
		this->write(string.data(), sizeof(char) * size);
	}

	template<>
	void operator << (const std::wstring& wstr)
	{
		size_t size = wstr.size();

		this->write(size);
		this->write(wstr.data(), sizeof(char) * size);
	}

	template< typename T >
	void operator << (const std::vector< T >& tVector)
	{
		size_t size = tVector.size();
		this->write(size);
		for (const T& element : tVector)
		{
			this->write(element);
		}
	}




	//Class 객체를 받을 때는 해당 클래스를 특수화 할 것


protected:
	void		write(const void* data, size_t size);

	template<class T>
	void write(T value)
	{
		static_assert(std::is_arithmetic< T >::value ||
			std::is_enum< T >::value,
			"Not Generic Data");

		write(&value, sizeof(value));
	}

	

};



class InputStream : public Stream
{
public:
	InputStream(char* buffer, size_t size);


	template< typename T >
	void operator >> (T& t)
	{
		this->read(t);
	}

	template<>
	void operator >> (std::string& str)
	{
		size_t size;
		this->read(size);

		str.resize(size);

		for (auto& c : str)
		{
			this->read(c);
		}

	}

	template<>
	void operator >> (std::wstring& wstr)
	{
		size_t size;
		this->read(size);

		wstr.resize(size);

		for (auto& c : wstr)
		{
			this->read(c);
		}
	}


	template< typename T >
	void operator >> (std::vector< T >& tVector)
	{
		size_t size;
		this->read(size);
		tVector.resize(size);
		for (auto& element : tVector)
		{
			this->read(element);
		}
	}


	//Class 객체를 받을 때는 해당 클래스를 특수화 할 것

protected:
	void		read(void* data, size_t size);


	template< typename T >
	void read(T& data)
	{
		static_assert(std::is_arithmetic< T >::value ||
			std::is_enum< T >::value,
			"Generic Read only supports primitive data types");

		read(&data, sizeof(data));
	}

};