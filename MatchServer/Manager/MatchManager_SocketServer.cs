using MatchServer.Contents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MatchServer
{
    public partial class MatchManager
    {
        object _serverLock = new object();


        static UInt32 ServerId = 1;

        Dictionary<string, List<SocketServer>> _servers = new Dictionary<string, List<SocketServer>>();

        public SocketServer FindServerId(string region, UInt32 serverId)
        {
            lock (_serverLock)
            {
                List<SocketServer> serverList;

                if (_servers.TryGetValue(region, out serverList))
                {
                    return serverList.Find(server => server._serverId == serverId);
                }

                else
                {
                    return null;
                }
            }
        }

        public SocketServer GetMinUserServer(string region)
        {
            lock (_serverLock)
            {
                List<SocketServer> serverList;

                if (_servers.TryGetValue(region, out serverList))
                {
                    return serverList.MinBy(server=>server._userCount);
                }

                else
                {
                    return null;
                }
            }
        }

        public RegistServerResponse RegistServer(RegistServerRequest request)
        {
            RegistServerResponse response = new RegistServerResponse();

            string region = request.ServerInfo.Region;
            string host = request.ServerInfo.Host;
            UInt32 port = request.ServerInfo.Port;

            SocketServer newServer = new SocketServer();
            newServer._host = host;
            newServer._port = port;
            newServer._serverId = ServerId;
            newServer._myRoomIds = new List<UInt64>();


            lock (_serverLock)
            {
                List<SocketServer> serverList;

                if (_servers.TryGetValue(region, out serverList))
                {
                    serverList.Add(newServer);
                }
                else
                {
                    serverList = new List<SocketServer>();

                    serverList.Add(newServer);
                    _servers.Add(region, serverList);
                }

                response.Result = true;
                response.ServerId = ServerId;

                ServerId++;
            }

            Console.WriteLine($" New Server Regist ID : {response.ServerId}");
                

            return response;

        }

        public Response RemoveServer(RemoveServerRequest request)
        {
            Response response = new Response();

            string region = request.Region;
            UInt32 serverId = request.ServerId;


            lock (_serverLock)
            {
                List<SocketServer> serverList;
                if (_servers.TryGetValue(region, out serverList))
                {
                    bool success = serverList.Remove(serverList.SingleOrDefault(x => x._serverId == serverId));

                    if (success) response.Result = true;
                }
            }

            Console.WriteLine($" Server Remove ID : {serverId}");

            return response;
        }

        public void RemoveServer(string region, UInt32 serverId)
        {
            lock (_serverLock)
            {
                List<SocketServer> serverList;
                if (_servers.TryGetValue(region, out serverList))
                {
                    bool success = serverList.Remove(serverList.SingleOrDefault(x => x._serverId == serverId));
                }
            }

            Console.WriteLine($" Server Remove ID : {serverId}");
        }


        public List<UInt64> GetRoomIdAtRegion(string region)
        {
            lock (_serverLock)
            {
                List<SocketServer> serverList;
                List<UInt64> roomIds = new List<UInt64>();
                if (_servers.TryGetValue(region, out serverList))
                {
                    foreach (SocketServer server in serverList)
                    {
                        roomIds = roomIds.Concat(server._myRoomIds).ToList();
                    }

                    return roomIds;

                }
                else return null;
            }
        }
    }
}
