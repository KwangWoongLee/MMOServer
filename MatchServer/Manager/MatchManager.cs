using Grpc.Core;
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
        static MatchManager _match = new MatchManager();
        public static MatchManager Instance { get { return _match; } }

        public Server _server;
        private MatchImpl _matchImpl;

        public void Init(UInt16 port)
        {
            _matchImpl = new MatchImpl();
            _server = new Server
            {
                Services = { MatchService.BindService(_matchImpl) },
                Ports = { new ServerPort("localhost", port, ServerCredentials.Insecure) },
                
            };
            
            _server.Start();

            Console.WriteLine("server listening on port " + port);
            Console.WriteLine("Press any key to stop the server...");
            Console.ReadKey();
        }

        public void Wait()
        {
            _server.ShutdownAsync().Wait();
        }
    }
}