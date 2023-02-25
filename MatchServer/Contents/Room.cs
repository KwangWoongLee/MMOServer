using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MatchServer.Contents
{
    public class Room
    {
        object _lock = new object();

        public string _title;
        public UInt64 _hostAidx;
        public UInt32 _serverId;
        public string _region;
        public UInt64 _roomId;
        public UInt64 _tcpRoomId;
        public UInt32 _mapId;
        public UInt32 _maxMemberCount;
        public UInt32 _minMemberCount;
        public UInt32 _nowMemberCount;
        public bool _roomLock = true;
        

        List<PUser> _users = new List<PUser>();

        public void Enter(PUser user)
        {
            lock (_lock)
            {
                _users.Add(user);
                ++_nowMemberCount;
            }
        }

        public void Leave(PUser user)
        {
            lock (_lock)
            {
                _users.Remove(user);
                --_nowMemberCount;
            }
        }

        public PUser GetUser(UInt64 aidx)
        {
            PUser user;

            lock (_lock)
            {
                user = _users.Find((user) =>
                {
                    return user.Aidx == aidx;
                });
            }

            
            return user;
        }

    }
}
