#include "stdafx.h"

#include <iostream>
#include <boost/asio.hpp>


int main()
{
	try {
		boost::asio::io_context io_context;
		boost::asio::ip::tcp::endpoint server_addr(boost::asio::ip::address::from_string("127.0.0.1"), 7700);
		boost::asio::ip::tcp::socket socket(io_context);
		boost::asio::connect(socket, &server_addr);

		
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

}