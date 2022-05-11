#include "stdafx.h"
#include "Session.h"

void Session::Init(SOCKET& socket) noexcept
{
	mSocket = socket;
}

//void Session::Disconnect()
//{
//}
//
//void Session::Send()
//{
//}
//
//void Session::OnRecvCompleted()
//{
//}
//
//void Session::OnSendCompleted()
//{
//}
