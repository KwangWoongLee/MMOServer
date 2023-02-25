using MatchServer.Contents;
using System;
using System.Collections.Generic;

namespace MatchServer
{
    public partial class MatchManager
    {
        object _roomLock = new object();

        static UInt64 RoomId = 1;
        Dictionary<UInt64, Room> _rooms = new Dictionary<UInt64, Room>();

        public CreateRoomResponse CreateRoom(CreateRoomRequest request)
        {
            CreateRoomResponse response = new CreateRoomResponse();

            SocketServer minServer = GetMinUserServer(request.Region);
            if (minServer == null) return null;

            string title = request.Title;
            UInt64 hostAidx = request.HostAidx;
            UInt32 mapId = request.MapId;
            UInt32 maxMemberCount = request.MaxMemberCount;
            UInt32 minMemberCount = request.MinMemberCount;

            Room newRoom = new Room();
            newRoom._title= title;
            newRoom._hostAidx= hostAidx;
            newRoom._mapId = mapId;
            newRoom._maxMemberCount = maxMemberCount;
            newRoom._minMemberCount = minMemberCount;
            newRoom._nowMemberCount = 0;

            lock (_roomLock)
            {
                if (_rooms.TryAdd(RoomId, newRoom))
                {
                    response.RoomId = RoomId;
                    newRoom._roomId = RoomId;
                    ++RoomId;
                };
            }


            minServer.PushRequest(newRoom);

            return response;
        }

        public bool RemoveRoom(UInt64 roomId)
        {
            bool success = false;
            lock (_roomLock)
            {
                success = _rooms.Remove(RoomId);
            }

            return success;
        }

        public Response RemoveRoom(RemoveRoomTCPRequest request)
        {
            Response response = new Response();

            lock (_roomLock)
            {
                if (_rooms.Remove(RoomId))
                {
                    response.Result = true;
                };
            }

            return response;
        }


        public EnterRoomResponse EnterRoom(EnterRoomRequest request)
        {
            EnterRoomResponse response = new EnterRoomResponse();
            response.Success = false;
            UInt64 aidx = request.Aidx;
            string name = request.Name;
            string region = request.Region;
            UInt64 roomId = request.RoomId;

            Room room;

            lock (_roomLock)
            {
                if (_rooms.TryGetValue(roomId, out room) == false)
                {
                    response.Message = "Not Found The Room";
                    return response;
                }
            }

            SocketServer ownerServer = FindServerId(room._region, room._serverId);
            if (ownerServer == null)
            {
                response.Message = "Owner Server Is Down";
                return response;
            }

            room.Enter(new PUser()
            {
                Aidx = aidx,
                Name = name,
                Region = region,
            });

            ownerServer._userCount++;

            response.Success = true;
            response.Host = ownerServer._host;
            response.Port = ownerServer._port;
            response.TcpRoomId = room._tcpRoomId;
            
            return response;
        }

        public Response LeaveRoom(LeaveRoomRequest request)
        {
            Response response = new Response();
            response.Result = false;
            UInt64 aidx = request.Aidx;
            UInt64 roomId = request.RoomId;

            Room room;
            lock (_roomLock)
            {
                if (_rooms.TryGetValue(roomId, out room) == false)
                {
                    return response;
                }
            }

            PUser user = room.GetUser(aidx);
            if (user == null)
            {
                return response;
            }

            room.Leave(user);

            response.Result = true;
            return response;
        }

        public GetRoomListResponse GetRoomList(GetRoomListRequest request) 
        {
            List<Room> rooms = new List<Room>();
            GetRoomListResponse response = new GetRoomListResponse();

            List<UInt64> roomIdsAtRegion = GetRoomIdAtRegion(request.Region);
            if (roomIdsAtRegion == null || roomIdsAtRegion.Count == 0) return response;

            lock (_roomLock)
            {
                foreach(UInt64 roomId in roomIdsAtRegion)
                {
                    Room room;
                    if (_rooms.TryGetValue(roomId, out room))
                    {
                        rooms.Add(room);
                    }
                    else return response;
                }
            }

            if (rooms == null || rooms.Count == 0) return response;

            foreach (Room room in rooms)
            {
                response.RoomInfos.Add(new RoomInfo()
                {
                    RoomId = room._roomId,
                    HostAidx = room._hostAidx,
                    Title = room._title,
                    MapId = room._mapId,
                    MaxMemberCount = room._maxMemberCount,
                    MinMemberCount = room._minMemberCount,
                    NowMemberCount = room._nowMemberCount,
                    Locked = room._roomLock,
                }) ;
            }
            

            return response;
        }

        public bool SetRoomUnLock(string region, UInt32 serverId, UInt64 roomId, UInt64 tcpRoomId)
        {
            Room room;
            lock (_roomLock)
            {
                if (_rooms.TryGetValue(roomId, out room) == false)
                {
                    return false;
                }

                room._region = region;
                room._roomLock = false;
                room._serverId = serverId;
                room._tcpRoomId = tcpRoomId;
            }

            SocketServer server = FindServerId(region, serverId);
            server._myRoomIds.Add(roomId);

            return true;
        }
    }
}