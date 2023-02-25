using Grpc.Core;
using MatchServer;
using System;

namespace GrpcService
{
    class Program
    {
        const int PORT = 5001;
        public static void Main(string[] args)
        {
            try
            {
                MatchManager.Instance.Init(PORT);

                MatchManager.Instance.Wait();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception encountered: {ex}");
            }        
        }
    }
}
