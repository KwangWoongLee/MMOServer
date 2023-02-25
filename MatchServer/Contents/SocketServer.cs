using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace MatchServer.Contents
{
    public class SocketServer
    {
        object _lock = new object();

        public string _host;
        public UInt32 _port;
        public UInt32 _serverId;
        public List<UInt64> _myRoomIds;
        public UInt64 _userCount;
        public Queue<Room> _createRoomQueue;

        public SocketServer()
        {
            _myRoomIds = new List<UInt64>();
            _createRoomQueue = new Queue<Room>();
        }
        ~SocketServer() {
            foreach (UInt64 roomId in _myRoomIds)
            {
                MatchManager.Instance.RemoveRoom(roomId);
            }
        }

        public List<Room> GetCreateRoomList()
        {
            List<Room> createRoomList = null;

            lock (_lock)
            {
                if (_createRoomQueue.Count > 0)
                    createRoomList = _createRoomQueue.ToList();

                _createRoomQueue.Clear();
            }

            return createRoomList;
        }

        public void PushRequest(Room room)
        {
            lock (_lock)
            {
                _createRoomQueue.Enqueue(room);
            }
        }




    }
}
