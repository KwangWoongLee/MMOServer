#include "stdafx.h"
#include "MatchManager.h"
#include "MatchClient.h"

std::shared_ptr<MatchManager> gMatchManager = std::make_shared<MatchManager>();

MatchManager::MatchManager()
{
}

MatchManager::~MatchManager()
{
	if (mServerAdded == true)
		mMatchClient->CallRemoveServer();

	mMatchClient = nullptr;
	
}

bool MatchManager::Init(std::string host, uint16 port)
{
	std::string sPort = to_string(port);
	auto serverAddress = host.append(":").append(sPort);
	
	mMatchClient = std::make_shared<MatchClient>(serverAddress , grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials()) );

	if (mMatchClient->CallRegistServer() == false)
	{
		return false;
	}
	if (mMatchClient->StartBiStream() == false)
	{
		return false;
	}

	/*if (mMatchClient->StartServerStream() == false)
	{
		return false;
	}*/

	mServerAdded = true;

	return true;
}


bool MatchManager::TestCreate()
{
	if (mMatchClient->CallRegistServer())
	{
		mServerAdded = true;
		return true;
	}
}

bool MatchManager::TestRemove()
{
	if (mMatchClient->CallRemoveServer())
	{
		mServerAdded = true;
		return true;
	}

	return false;
}