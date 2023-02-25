#pragma once

#include "Proto/match.pb.h"
#include "Proto/match.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

using namespace grpc;

class MatchClient : public MatchService::Service
{
public:
	MatchClient(std::string serverAddress, std::shared_ptr<ChannelInterface> channel)
		: mServerAddress(serverAddress),
		mStub(MatchService::NewStub(channel))
	{};

	bool CallRegistServer();
	bool CallRemoveServer();

	//bool StartServerStream();
	//static void CreateRoomJob();
	static std::unique_ptr<grpc::ClientReader<CreateRoomTCPResponse>> sServerStreamReader;


	bool StartBiStream();
	static void CreateRoomJobBi();
	static std::unique_ptr<grpc::ClientReaderWriter<CreateRoomTCPRequest, CreateRoomTCPResponse>> sBiStreamRW;
	static uint32		sMatchServerId;



private:
	std::string mServerAddress;

	std::shared_ptr<ChannelInterface> mChannel = nullptr;
	std::unique_ptr<MatchService::Stub> mStub = nullptr;
	ClientContext	mServerStreamContext; 
	CreateRoomTCPRequest mServerStreamRequest;


	ClientContext	mBiStreamContext;
	CreateRoomTCPRequest mBiStreamRequest;
};
