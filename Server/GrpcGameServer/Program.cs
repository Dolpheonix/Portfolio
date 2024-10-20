// See https://aka.ms/new-console-template for more information
using Grpc.Core;
using GrpcGameServer;
using ProtoObject;

class Program
{
    static bool bRunning = true;

    static int Main(string[] args)
    {
        var service = new GameServiceImpl();
        Server server = new Server
        {
            Services = { GameService.BindService(service) },
            Ports = { new ServerPort("localhost", 30051, ServerCredentials.Insecure) }
        };

        server.Start();
        Console.WriteLine("Grpc Server: listening on port 30051...");
        while(bRunning)
        {
            var command = Console.ReadLine()?.ToLower();
            switch(command)
            {
                case "q":
                    server.ShutdownAsync().Wait();
                    bRunning = false;
                    break;
                case "l":
                    service.ListClients();
                    break;
                default:
                    Console.WriteLine("Undefined command.");
                    break;
            }
        }

        return 0;
    }
}