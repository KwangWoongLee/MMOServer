#pragma once

#include "Proto/match.pb.h"
#include "Proto/match.grpc.pb.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <thread>
#include <iostream>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

class GRPC {
public:
    GRPC(std::shared_ptr<Channel> channel) : _stub{ MatchService::NewStub(channel) } {}

    //Empty AddServer() {
    //    ServerInfo request;
    //    request.set_host("localhost");
    //    request.set_port(5002);
    //    request.set_region("kor");

    //    // Create an RPC object
    //    Empty response;
    //    ClientContext context;
    //    CompletionQueue queue;
    //    Status status;
    //    std::unique_ptr<ClientAsyncResponseReader<Empty>> rpc;
    //    rpc = _stub->PrepareAsyncAddServer(&context, request, &queue);
    //    // Initiate the RPC call
    //    rpc->StartCall();

    //    // Request to update the server's response and the call status upon completion of the RPC
    //    rpc->Finish(&response, &status, (void*)1);

    //    // Complete the RPC call
    //    void* tag;
    //    bool ok = false;
    //    if (queue.Next(&tag, &ok) && ok && tag == (void*)1) {
    //        if (status.ok()) {
    //            return response;
    //        }
    //        else {
    //            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
    //            //return "RPC failed";
    //            return Empty();
    //        }
    //    }
    //    else {
    //        std::cerr << "Something went wrong" << std::endl;
    //        abort();
    //    }
    //}
private:
    std::unique_ptr<MatchService::Stub> _stub;
};