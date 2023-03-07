#include "stdafx.h"
#include "MatchClient.h"
#include "RoomManager.h"

std::unique_ptr<grpc::ClientReader<CreateRoomTCPResponse>> MatchClient::sServerStreamReader = nullptr;
std::unique_ptr<grpc::ClientReaderWriter<CreateRoomTCPRequest, CreateRoomTCPResponse>> MatchClient::sBiStreamRW = nullptr;
uint32		MatchClient::sMatchServerId = 0;

bool MatchClient::CallRegistServer()
{
    RegistServerRequest request;
    RegistServerResponse response;

	if (auto pos = mServerAddress.find_first_of(":"))
	{
		auto host = mServerAddress.substr(0, pos);
		auto port = static_cast<uint16>(stoi(mServerAddress.substr(pos + 1, mServerAddress.size() - pos - 1)));
        mServerStreamRequest.set_region("kor");

        ServerInfo* serverInfo = request.mutable_serverinfo();
        serverInfo->set_host("127.0.0.1");
        serverInfo->set_port(7777);
        serverInfo->set_region(mServerStreamRequest.region());


        ClientContext context;
        CompletionQueue queue;
        Status status;
        std::unique_ptr<ClientAsyncResponseReader<RegistServerResponse>> rpc;
        rpc = mStub->PrepareAsyncRegistServer(&context, request, &queue);
        rpc->StartCall();

        rpc->Finish(&response, &status, (void*)1);

        void* tag;
        bool ok = false;
        if (queue.Next(&tag, &ok) && ok && tag == (void*)1) {
            if (status.ok() == false) {
                std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
                ASSERT_CRASH(false);
            }
            else {
                cout << "My Match Server Id : " << response.serverid() << endl;
                sMatchServerId = response.serverid();
            }
        }
        else {
            std::cerr << "Something went wrong" << std::endl;
            ASSERT_CRASH(false);
        }
    }

    return response.result();
}

bool MatchClient::CallRemoveServer()
{
    RemoveServerRequest request;
    Response response;

    request.set_serverid(mServerStreamRequest.serverid());
    request.set_region(mServerStreamRequest.region());

    ClientContext context;
    CompletionQueue queue;
    Status status;
    std::unique_ptr<ClientAsyncResponseReader<Response>> rpc;
    rpc = mStub->PrepareAsyncRemoveServer(&context, request, &queue);
    rpc->StartCall();

    rpc->Finish(&response, &status, (void*)1);

    void* tag;
    bool ok = false;
    if (queue.Next(&tag, &ok) && ok && tag == (void*)1) {
        if (status.ok() == false) {
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            ASSERT_CRASH(false);
        }
    }
    else {
        std::cerr << "Something went wrong" << std::endl;
        ASSERT_CRASH(false);
    }

    return response.result();
}


//bool MatchClient::StartServerStream()
//{
//    auto rpc(mStub->CreateRoomTCP(&mServerStreamContext, mServerStreamRequest));
//    sServerStreamReader = std::move(rpc);
//
//    return true;
//}
//
//void MatchClient::CreateRoomJob()
//{
//    CreateRoomTCPResponse response;
//    while (sServerStreamReader->Read(&response)) {
//        cout << "Aidx  : " << response.hostaidx() << std::endl;
//        cout << "MapId : " << response.mapid() << std::endl;
//        cout << "Max   : " << response.maxmembercount() << std::endl;
//        cout << "Min   : " << response.minmembercount() << std::endl;
//
//    }
//
//    Status status = sServerStreamReader->Finish();
//    if (status.ok() == false) {
//        std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
//        ASSERT_CRASH(false);
//    }
//}



bool MatchClient::StartBiStream()
{
    auto rpc(mStub->CreateRoomTCP(&mBiStreamContext));
    sBiStreamRW = std::move(rpc);

    CreateRoomTCPRequest request;
    request.set_region("kor");
    request.set_serverid(sMatchServerId);

    sBiStreamRW->Write(request);

    return true;
}

void MatchClient::CreateRoomJobBi()
{

    CreateRoomTCPResponse response;
    while (sBiStreamRW->Read(&response)) {
        //uint64 newRoomId = gRoomManager->Add(response.hostaidx(), response.mapid(), response.maxmembercount(), response.minmembercount());
        //cout << "货肺款 规 积己 : " << newRoomId << endl;

        CreateRoomTCPRequest request;
        //request.set_region("kor");
        //request.set_serverid(sMatchServerId);
        //request.set_roomid(response.roomid());
        //request.set_tcproomid(newRoomId);

        sBiStreamRW->Write(request);
    }

    Status status = sBiStreamRW->Finish();
    if (status.ok() == false) {
        std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
        ASSERT_CRASH(false);
    }
}
