using Google.Protobuf.WellKnownTypes;
using Grpc.Core;
using Grpc.Net.Client;
using MatchServer.Contents;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MatchServer
{
    public class MatchImpl : MatchService.MatchServiceBase
    {
        object _lock = new object();


        #region NODE => MATCH
        public override Task<CreateRoomResponse> CreateRoom(CreateRoomRequest request, ServerCallContext context)
        {
            return Task.FromResult(MatchManager.Instance.CreateRoom(request));
        }

        public override Task<EnterRoomResponse> EnterRoom(EnterRoomRequest request, ServerCallContext context)
        {
            return Task.FromResult(MatchManager.Instance.EnterRoom(request));
        }

        public override Task<Response> LeaveRoom(LeaveRoomRequest request, ServerCallContext context)
        {
            return Task.FromResult(MatchManager.Instance.LeaveRoom(request));
        }

        //public override Task<Response> StartGame(StartGameRequest request, ServerCallContext context)
        //{
        //    return Task.FromResult(MatchManager.Instance.StartGame(request));
        //}

        public override Task<GetRoomListResponse> GetRoomList(GetRoomListRequest request, ServerCallContext context)
        {
            return Task.FromResult(MatchManager.Instance.GetRoomList(request));
        }
        #endregion


        #region MATCH => NODE

        #endregion


        #region TCP => MATCH
        public override Task<RegistServerResponse> RegistServer(RegistServerRequest request, ServerCallContext context)
        {
            return Task.FromResult(MatchManager.Instance.RegistServer(request));
        }

        public override Task<Response> RemoveServer(RemoveServerRequest request, ServerCallContext context)
        {
            return Task.FromResult(MatchManager.Instance.RemoveServer(request));
        }
        #endregion


        #region MATCH => TCP

        //public override async Task CreateRoomTCP(CreateRoomTCPRequest request, IServerStreamWriter<CreateRoomTCPResponse> responseStream, ServerCallContext context)
        //{
        //    while (!context.CancellationToken.IsCancellationRequested)
        //    {
        //        string region = request.Region;
        //        UInt32 serverId = request.ServerId;

        //        SocketServer server = MatchManager.Instance.FindServerId(request.Region, request.ServerId); // LOCK
        //        List<CreateRoomRequest> toCreatingRoomList = server.GetCreateRoomList();
        //        if (toCreatingRoomList == null) continue;

        //        foreach (var nodeCreateRoomRequest in toCreatingRoomList)
        //        {
        //            CreateRoomTCPResponse response = new CreateRoomTCPResponse();

        //            response.HostAidx = nodeCreateRoomRequest.HostAidx;
        //            response.Title = nodeCreateRoomRequest.Title;
        //            response.MapId = nodeCreateRoomRequest.MapId;
        //            response.MaxMemberCount = nodeCreateRoomRequest.MaxMemberCount;
        //            response.MinMemberCount = nodeCreateRoomRequest.MinMemberCount;
        //            await responseStream.WriteAsync(response);
        //        }
        //    }
        //}

        public override async Task CreateRoomTCP(IAsyncStreamReader<CreateRoomTCPRequest> requestStream,
    IServerStreamWriter<CreateRoomTCPResponse> responseStream, ServerCallContext context)
        {
            string region = "";
            UInt32 serverId = 0;

            await foreach (var message in requestStream.ReadAllAsync())
            {
                region = message.Region;
                serverId = message.ServerId;
                break;
            }


            var readTask = Task.Run(async () =>
            {
                await foreach (var message in requestStream.ReadAllAsync())
                {
                    Console.WriteLine($" New Room({message.RoomId}) Create By ID : {message.ServerId}");

                    MatchManager.Instance.SetRoomUnLock(message.Region, message.ServerId, message.RoomId, message.TcpRoomId);
                }

                Console.WriteLine($" {serverId} Server Out!");

                lock (_lock)
                {
                    MatchManager.Instance.RemoveServer(region, serverId);
                }
                
            });

            while (!readTask.IsCompleted)
            {
                if (region == "") continue;
                List<Room> toCreatingRoomList;

                lock (_lock)
                {
                    SocketServer server = MatchManager.Instance.FindServerId(region, serverId); // LOCK
                    if (server == null) return;
                        
                    toCreatingRoomList = server.GetCreateRoomList();
                    if (toCreatingRoomList == null) continue;
                }


                foreach (Room room in toCreatingRoomList)
                {
                    CreateRoomTCPResponse response = new CreateRoomTCPResponse();

                    response.HostAidx = room._hostAidx;
                    response.Title = room._title;
                    response.MapId = room._mapId;
                    response.MaxMemberCount = room._maxMemberCount;
                    response.MinMemberCount = room._minMemberCount;
                    response.RoomId = room._roomId;
                    await responseStream.WriteAsync(response);
                }
            }
        }

        #endregion
    };
}
